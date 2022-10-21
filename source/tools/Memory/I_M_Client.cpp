    /*
 * Copyright 2002-2019 Intel Corporation.
 * 
 * This software is provided to you as Sample Source Code as defined in the accompanying
 * End User License Agreement for the Intel(R) Software Development Products ("Agreement")
 * section 1.L.
 * 
 * This software and the related documents are provided as is, with no express or implied
 * warranties, other than those that are expressly stated in the License.
 */

/*! @file
 *  This file contains an ISA-portable PIN tool for functional simulation of
 *  instruction+data TLB+cache hierarchies
 */

using namespace std;
#define INS_LIMIT 100000
#define TLB_PENALTY 20
#define L1_PENALTY 3
#define L2_PENALTY 15

#include "pin.H"
#include "instlib2.H"
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
typedef UINT32 CACHE_STATS; // type of cache hit/miss counters
#include "pin_cache.H"
#include <boost/preprocessor.hpp>
#include <unistd.h>

std::ofstream TraceFi;
std::ofstream ins_stat;

//Get process number from the script
KNOB<UINT32> KnobProcessNumber(KNOB_MODE_WRITEONCE, "pintool", "P", "1", "Process number");
//Get node number from the script
KNOB<UINT32> KnobNodeNumber(KNOB_MODE_WRITEONCE, "pintool", "N", "1", "Node number");
int Procid,Nodeid;

using std::hex;
using std::string;
using std::ios;
using std::dec;
using std::endl;


// Force each thread's data to be in its own data cache line so that
// multiple threads do not contend for the same data cache line.
// This avoids the false sharing problem.
#define PADSIZE 56

INT32 numThreads = 0;

PIN_LOCK lock;
PIN_MUTEX *Mutex,*Mutex1;

// a running count of the instructions
class thread_data_t
{
  public:
    thread_data_t() : _count(0) {}
    UINT64 _count;
    UINT8 _pad[PADSIZE];
};	

// key for accessing TLS storage in the threads. initialized once in main()
static  TLS_KEY tls_key = INVALID_TLS_KEY;

//function to access thread−specific data
thread_data_t* get_tls(THREADID threadid)
{	
     thread_data_t* tdata = static_cast<thread_data_t*>(PIN_GetThreadData(tls_key , threadid));
     return tdata;
}

VOID ThreadStart(THREADID threadid, CONTEXT *ctxt, INT32 flags, VOID *v)
{
    PIN_GetLock(&lock, threadid+1);
    numThreads++;
    PIN_ReleaseLock(&lock);
    thread_data_t* tdata = new thread_data_t;
    PIN_SetThreadData(tls_key, tdata, threadid);
/*    if (PIN_SetThreadData(tls_key, tdata, threadid) == FALSE)
    {
        cerr << "PIN_SetThreadData failed" << endl;
        PIN_ExitProcess(1);
    }
*/
}

INSTLIB::ICOUNT icount;
UINT64 ram_count=0;
UINT64 ins_count = 0;
UINT32 window_count=0;

struct Instruction_Buffer
{
    UINT32 size;
    CACHE_BASE::ACCESS_TYPE accessType;
    CHAR r;
    ADDRINT addr;
    UINT32 threadid;
    UINT32 buf_type;
    int procid;
    int index;
};

struct window
{
   struct Instruction_Buffer buf[2000000];
}*app;

UINT64 *buf_no,*win;
bool *main_start;
UINT64 ram_count1=0;


UINT64 a=0,b=0;

static VOID FillBuffer(UINT32 threadid, ADDRINT addr, UINT32 size, CACHE_BASE::ACCESS_TYPE accessType, CHAR r, int buf_type)
{
    PIN_MutexLock(Mutex);

    if((*win)>=500000000)
    {
        UINT32 c = icount.MultiThreadCount();
        icount.SetMultithreadCount(0);
        ins_count=ins_count+c;
        PIN_MutexUnlock(Mutex);
        PIN_ExitApplication(0);
    }
    
    if(*buf_no==2000000)
    {      
        TraceFi<<"Node"<<Procid<<" Miss\n";
        //*buf_no=0;
        PIN_MutexUnlock(Mutex);
        return;  
    }

    *win=*win+1;
    a++;

    if(a==1)
        icount.SetMultithreadCount(0);
       
    if(buf_type==1 || buf_type==2 || buf_type==3)
    {
        app->buf[*buf_no].threadid=threadid;
        app->buf[*buf_no].procid=Procid;
        app->buf[*buf_no].addr=addr;
        app->buf[*buf_no].r=r;
    }
        
    if(buf_type==2 || buf_type==3)
    {
        app->buf[*buf_no].size=size;
        app->buf[*buf_no].accessType=accessType;
    }
    app->buf[*buf_no].buf_type=buf_type;
    app->buf[*buf_no].index=*buf_no;
    (*buf_no)++;
    PIN_MutexUnlock(Mutex);
}

UINT64 num=0;
static VOID InsRef(UINT32 threadid, ADDRINT addr)
{
    //if(numThreads<2 || 
    if(*main_start==false)
        return;
    CHAR r='F';
    CACHE_BASE::ACCESS_TYPE accessType= CACHE_BASE::ACCESS_TYPE_LOAD;
    FillBuffer(threadid,addr,1,accessType,r,1);
}


static VOID MemRefMulti(UINT32 threadid, ADDRINT addr, UINT32 size, CACHE_BASE::ACCESS_TYPE accessType, CHAR r)
{
    //if(numThreads<2 || 
    if(*main_start==false)
        return;
    FillBuffer(threadid,addr,size,accessType,r,2);
}


static VOID MemRefSingle(UINT32 threadid, ADDRINT addr, UINT32 size, CACHE_BASE::ACCESS_TYPE accessType, CHAR r)
{
    //if(numThreads<2 || 
    if(*main_start==false)
        return;
    FillBuffer(threadid,addr,size,accessType,r,3);
}


static VOID Instruction(INS ins, VOID *v)
{

    // all instruction fetches access I-cache
    INS_InsertCall(
        ins, IPOINT_BEFORE, (AFUNPTR)InsRef,
    IARG_THREAD_ID,
        IARG_INST_PTR,
        IARG_END);

    if (!INS_IsStandardMemop(ins)) return;
    if (INS_MemoryOperandCount(ins) == 0) return;
    ;

    UINT32 readSize = 0, writeSize = 0;
    UINT32 readOperandCount = 0, writeOperandCount = 0;

    for (UINT32 opIdx = 0; opIdx < INS_MemoryOperandCount(ins); opIdx++)
    {
        if (INS_MemoryOperandIsRead(ins, opIdx))
        {
            readSize = INS_MemoryOperandSize(ins, opIdx);
            readOperandCount++;
            break;
        }
        if (INS_MemoryOperandIsWritten(ins, opIdx))
        {
            writeSize = INS_MemoryOperandSize(ins, opIdx);
            writeOperandCount++;
            break;
        }
    }

    if (readOperandCount > 0)
    {
        const AFUNPTR countFun = (readSize <= 4 ? (AFUNPTR)MemRefSingle : (AFUNPTR)MemRefMulti);

        // only predicated-on memory instructions access D-cache
        INS_InsertPredicatedCall(ins, IPOINT_BEFORE, countFun, IARG_THREAD_ID, IARG_MEMORYREAD_EA, 
                                IARG_MEMORYREAD_SIZE, IARG_UINT32,CACHE_BASE::ACCESS_TYPE_LOAD, 
                                IARG_UINT32, 'R', IARG_END);
    }

    if (writeOperandCount > 0)
    {
        const AFUNPTR countFun = (writeSize <= 4 ? (AFUNPTR)MemRefSingle : (AFUNPTR)MemRefMulti);

        // only predicated-on memory instructions access D-cache
        INS_InsertPredicatedCall(ins, IPOINT_BEFORE, countFun, IARG_THREAD_ID,IARG_MEMORYWRITE_EA, 
                                 IARG_MEMORYWRITE_SIZE, IARG_UINT32, CACHE_BASE::ACCESS_TYPE_STORE, 
                                 IARG_UINT32, 'W', IARG_END);
    }
}

//Shared memory keys and IDs
key_t ShmKEY1,ShmKEY2,ShmKEY3,ShmKEY4,ShmKEY5,ShmKEY6;
int ShmID1,ShmID2,ShmID3,ShmID4,ShmID5,ShmID6;

static VOID Fini(int code, VOID * v)
{     
    PIN_MutexLock(Mutex1);
    ins_stat<<"\nTotal memory accesses(to cache) by CPU, traced in Node-"<<Nodeid<<" Proc-"<<Procid<<" : "<<a;
    ins_stat<<"\nTotal_instructions traced in Node-"<<Nodeid<<" Proc-"<<Procid<<" : "<<ins_count<<endl;
    PIN_MutexUnlock(Mutex1);

    shmdt(app);
    shmdt(buf_no);
    shmdt(win);
    shmdt(Mutex);
    shmdt(main_start);
    shmdt(Mutex1);
    shmctl(ShmID1, IPC_RMID, NULL);
    shmctl(ShmID2, IPC_RMID, NULL);
    shmctl(ShmID3, IPC_RMID, NULL);
    shmctl(ShmID4, IPC_RMID, NULL);
    shmctl(ShmID5, IPC_RMID, NULL);
    shmctl(ShmID6, IPC_RMID, NULL);

    PIN_MutexFini(Mutex);
    PIN_MutexFini(Mutex1);
    TraceFi.close();
}

extern int main(int argc, char *argv[])
{
    PIN_Init(argc, argv);
    
	//Initialize lock
	PIN_InitLock(&lock);
	
    Procid=KnobProcessNumber.Value();
    Nodeid=KnobNodeNumber.Value();

    ShmKEY1 = Nodeid*10+1;
    ShmID1 = shmget(ShmKEY1, sizeof(window), IPC_CREAT | 0666);
    app = (window *)shmat(ShmID1, NULL, 0);


    ShmKEY2 = Nodeid*10+2;
    ShmID2 = shmget(ShmKEY2, sizeof(UINT64), IPC_CREAT | 0666);
    buf_no = (UINT64 *)shmat(ShmID2, NULL, 0);

    ShmKEY3 = Nodeid*10+3;
    ShmID3 = shmget(ShmKEY3, sizeof(UINT64), IPC_CREAT | 0666);
    win = (UINT64 *)shmat(ShmID3, NULL, 0);
    
    //Initialize the mutexes
    ShmKEY4 = Nodeid*10+4;
    ShmID4 = shmget(ShmKEY4, sizeof(PIN_MUTEX), IPC_CREAT | 0666);
    Mutex = (PIN_MUTEX *)shmat(ShmID4, NULL, 0);
    PIN_MutexInit(Mutex);

    //Variable to know client processes about start of main
    ShmKEY5 = Nodeid*10+5;
    ShmID5 = shmget(ShmKEY5, sizeof(UINT64), IPC_CREAT | 0666);
    main_start = (bool *)shmat(ShmID5, NULL, 0);
 
    //Initialize the mutexes
    ShmKEY6 = 1000;
    ShmID6 = shmget(ShmKEY6, sizeof(PIN_MUTEX), IPC_CREAT | 0666);
    Mutex1 = (PIN_MUTEX *)shmat(ShmID6, NULL, 0);
    PIN_MutexInit(Mutex1);

    icount.Activate(INSTLIB::ICOUNT::ModeBoth);
	
    //Arguments for StringDec
    UINT32 x=1;
    char y='P';
    string nID=StringDec(Nodeid,x,y);
    
    string d1="Output/Node" + nID;
    mkdir("Output",0777);
    const char *dir1=d1.c_str();
    mkdir(dir1,0777);
    
    string f2=d1+"/TraceFi.trc";
    
    const char *file1=f2.c_str();
    
    TraceFi.open(file1);
    ins_stat.open("Ins_trace_info.log",ios::app);
    
	//Obtain a key for TLS storage
	tls_key=PIN_CreateThreadDataKey(0);

	//Register ThreadStart to be called when a thread starts
	PIN_AddThreadStartFunction(ThreadStart,0);

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();

    return 0; // make compiler happy
}
