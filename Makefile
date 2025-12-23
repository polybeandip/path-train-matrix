RGB_LIB_DISTRIBUTION = matrix
RGB_LIBRARY_NAME     = rgbmatrix
RGB_INCDIR  = $(RGB_LIB_DISTRIBUTION)/include
RGB_LIBDIR  = $(RGB_LIB_DISTRIBUTION)/lib
RGB_LIBRARY = $(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).a
RGB_LDFLAGS = -L$(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME)

LDFLAGS += $(RGB_LDFLAGS) -lrt -lm -lpthread -lcurl -lboost_log
CXXFLAGS = -Wextra -Wall -O3 -std=c++20 -I$(RGB_INCDIR) -DBOOST_LOG_DYN_LINK

EXEC_OBJS = main.o pathpoller.o utils.o train.o

path: $(EXEC_OBJS) $(RGB_LIBRARY)
	$(CXX) $(CXXFLAGS) $(EXEC_OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(RGB_LIBRARY):
	$(MAKE) -C $(RGB_LIBDIR)
