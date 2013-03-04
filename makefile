SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SRCS))
CXX = g++
LD_FLAGS = `pkg-config --libs OGRE OGRE-Terrain OIS bullet` -lboost_system -lboost_thread-mt -lenet 
CC_FLAGS = -g `pkg-config --cflags OGRE OGRE-Terrain OIS bullet` -I./include/rapidxml
TITLE = ZombieClient

$(TITLE): $(OBJS)
	$(CXX) -o $@ $^ $(LD_FLAGS)
%.o: %.cpp
	$(CXX) $(CC_FLAGS) -c $<

clean:
	rm *.o
	rm $(TITLE)

run:
	./$(TITLE)
