# Data-Structure-honor-Final-Project
## 复旦大学 2023年秋季学期 数据结构（荣誉课）
**期末Project：绘制地图。**

_基本要求：_

①从open street map网站上下载一定范围内的地图。

②展示地图；

③查询地图上2点间的距离。


_扩展要求：_

①鼠标中键可以对当前地图界面截图。

（待实现）②输入一个地名，通过模糊搜索在地图上找到这个点。

③底端弄一个窗口栏

④查询公交线路，最短路可以选择是否偏向乘坐公交车

（待实现）也可以实现不同的交通方式(tagk=highway，对应tagv=pedestrian/footway/steps人行道，secondary二级公路，residental主干道)

⑤初始ui界面选择地图

⑥选择任何一个点，可以自动寻找到最近的路径

（待实现）⑦显示文字更有意义

（待实现）⑧写一个分块，用来快速找到要渲染的点

未完待续。。。

_待修复功能：_

①文字无法正常读入

②改用putimage_rotatezoom来展示地图（好像不太行）


------

**编程提示：**

1、读取地图文件用到了tinyxml库，整个都在项目里面。

2、绘图用到了ege库，使用流程如下：

    (1) 先在官网下载。https://xege.org/manual/index.htm

    (2) 下载后，配置。https://blog.csdn.net/qq_39151563/article/details/100161986#comments_29864761

3、关于文件夹：

    ./code：此project所有的代码。
    
    ./tinyxml：tinyxml库的所有代码
    
    ./map：地图文件，格式为.osm，在open street map网站上下载的。https://www.openstreetmap.org/
