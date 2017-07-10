INCLUDES = -Iinclude/ -I/opt/rocm/include/hsa/ -I/home/paul/Source/rocm/GPA/Include/
LIBDIRS = -L/opt/rocm/hsa/lib/ -L/home/paul/Source/rocm/GPA/Bin/Linx64/
LIBS = -ldl -lpthread -llttng-ust -lhsa-runtime-tools64 -lGPUPerfAPIHSA
MODULES = call_stack kernel_times queue_profiling perf_counters

all: tracepoints util $(MODULES)

tracepoints:
	gcc -c -o obj/hsa_tracepoints.o -fPIC src/hsa_tracepoints.c $(INCLUDES)

util:
	g++ -c -std=c++11 -o obj/util.o -fPIC src/util.cpp $(INCLUDES)

$(MODULES):
	g++ -std=c++11 -shared -o lib/hsa_$@.so obj/hsa_tracepoints.o obj/util.o -fPIC src/hsa_$@.cpp $(INCLUDES) $(LIBDIRS) $(LIBS)

clean:
	$(RM) obj/*.o lib/*.so
