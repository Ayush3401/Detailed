#include<iostream>



void Transaction_Layer(int *addr, int burst_length);
void Network_Layer(int *addr, int burst_length);
void Link_Layer(int *addr, int burst_length);
void Physical_Layer(int *addr, int burst_length);




//Return 1 for Local, 0 for Remote
void Local_Remote Map(int *addr)
{
		
}

//Handle Local memory access 
void Local_Access()
{

}


//Hanlde Remote memory Access
void Remote_Access()
{	int a=1;
	int *add=&a;
	int **addr=add;
	int burst_length=1;

//Virtual to Physical Addr Map & Packet preparation


//Receive request from local bus cache-line or bigger size
	Transaction_Layer(*addr, burst_length);

//Map to correct shared memory pool		//Virtual to Physical is optional here 
	Network_Layer(*addr, burst_length);

//To appropriate link-layer ports
	Link_Layer(*addr, burst_length);

//
	Physical_Layer(*addr, burst_length);

//To Calculate Network Time
	//Memory_Request_Network_Time(*addr, burst_length)

}




void main()
{
	
}