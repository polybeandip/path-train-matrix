RGB_LIB_DISTRIBUTION = matrix
RGB_INCDIR = $(RGB_LIB_DISTRIBUTION)/include
RGB_LIBDIR = $(RGB_LIB_DISTRIBUTION)/lib
RGB_LIBRARY_NAME = rgbmatrix
RGB_LIBRARY = $(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).a
LDFLAGS += -L$(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME) -lrt -lm -lpthread

CXXFLAGS = -Wextra -Wall -O3 -I$(RGB_INCDIR)

EXEC_OBJS = main.o flipframecanvas.o drawutils.o

path: $(EXEC_OBJS) $(RGB_LIBRARY)
	$(CXX) $(CXXFLAGS) $(EXEC_OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(RGB_LIBRARY):
	$(MAKE) -C $(RGB_LIBDIR)
