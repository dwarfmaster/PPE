PROG=testcv.prog
# Add -pg for use with gprof
CXXFLAGS=-Wall -Wextra -pedantic -std=c++11 `pkg-config --cflags opencv`
LDFLAGS=-lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_ocl -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videostab -ltbb /lib64/libXext.so /lib64/libX11.so /lib64/libICE.so /lib64/libSM.so /lib64/libGL.so /lib64/libGLU.so -lrt -lpthread -lm -ldl 
OBJS=cameras.o abod.o main2.o

all : $(OBJS)
	g++ $(CXXFLAGS) -o $(PROG) $^ $(LDFLAGS)

%.o : %.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

clean :
	rm -vf *.o

rec : clean all

.PHONY:rec clean all


