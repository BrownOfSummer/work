包含了reading, writing, 和color-coding .flo images
flowIO.cpp中有读写.flo文件的示例代码；

.flo文件是用来评估光流的文件格式
保存了2个浮点数图像分别代表了: u(水平)， v(垂直)
浮点数采用little-endian模式，绝对值大于1e9会被认作是"未知"
组织形式：
    0-3: "PIEH", 在little endian 中正好是202021.25, 用来检查是否正确；
    4-7: width, 整数形式
    8-11: height, 整数形式
    12-end: 数据(总共 width*height*2*4 bytes)
        存储的是u,v的浮点值，按照行顺序组织：u[row0, col0], v[row0, col0], u[row0, col1], v[row0, col1], ...
  
可以用color_flow 来生成color coding
用colortest 来可视化

编译：
    cd imageLib
    make
    cd ..
    make
    ./colortest 10 colors.png

所有的代码来自于：http://vision.middlebury.edu/flow/data/
./color_flow result.flo output.png 得到彩色光流图
