INCLUDES = -Iinclude/ -I/opt/rocm/hsa/include/ -I/home/paul/Source/rocm/GPA/Src/GPUPerfAPI-Common
LIBDIRS = -L/opt/rocm/hsa/lib/ -L/home/paul/Source/rocm/GPA/Src/GPUPerfAPIHSA
LIBS = -ldl -llttng-ust -lhsa-runtime-tools64 -lGPUPerfAPIHSA
MODULES = call_stack queue_profiling perf_counters perf_counters_new

all: tracepoints $(MODULES)

tracepoints:
	gcc -c -o obj/hsa_tracepoints.o -fPIC src/hsa_tracepoints.c $(INCLUDES)

$(MODULES):
	g++ -std=c++11 -shared -o lib/hsa_$@.so obj/hsa_tracepoints.o -fPIC src/hsa_$@.cpp $(INCLUDES) $(LIBDIRS) $(LIBS)

clean:
	$(RM) obj/*.o lib/*.so
