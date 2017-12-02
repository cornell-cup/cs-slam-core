# mac compile

# Dev build with server
# g++ -std=c++11 -I/usr/local/Cellar/opencv3/HEAD-642e4d9/include -L/usr/local/Cellar/opencv3/HEAD-642e4d9/lib/ -g -rdynamic -o build/build src/*.cpp src/server/*.cpp src/server/*.c -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_video -lopencv_calib3d -lopencv_videoio

# GUI version without
g++ -std=c++11 -I/usr/local/Cellar/opencv3/3.3.0_3/include -L/usr/local/Cellar/opencv3/3.3.0_3/lib/ -framework OpenGL -framework GLUT -g -rdynamic -o build/build src/*.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_video -lopencv_calib3d -lopencv_videoio
