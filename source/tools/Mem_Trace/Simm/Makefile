
#tell the linker the rpath so that we don't have to muck with LD_LIBRARY_PATH, etc
read: read.cpp
	$(CXX) -g -o read1 read.cpp -I./DRAMSim2/ -L./DRAMSim2/ -ldramsim -Wl,-rpath=./DRAMSim2/ -lpthread

clean: 
	rm read
