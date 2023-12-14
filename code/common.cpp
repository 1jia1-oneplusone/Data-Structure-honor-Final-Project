#define ll long long
#define dbl double

/*
把map_bw的坐标改造成map_col的坐标 bw to col: (yyyy/bw2col_w) , (height_map_col-xxxx/bw2col_h)
同时要考虑到彩色地图框的移动，这里的坐标就全部采用彩色地图的了: 
	((yyyy/bw2col_w)-move_dx)*move_time[move_tm] , ((height_map_col-xxxx/bw2col_h)-move_dy)*move_time[move_tm]
为了提高精度，故使用分配律，提前做乘法再做除法。
*/
#define b2cxy(xxxx,yyyy) ((yyyy*move_time[move_tm]/bw2col_w)-move_dx*move_time[move_tm]) ,\
						 ((height_map_col*move_time[move_tm]-xxxx*move_time[move_tm]/bw2col_h)-move_dy*move_time[move_tm])
#define b2cx(yyyy) ((yyyy*move_time[move_tm]/bw2col_w)-move_dx*move_time[move_tm])
#define b2cy(xxxx) ((height_map_col*move_time[move_tm]-xxxx*move_time[move_tm]/bw2col_h)-move_dy*move_time[move_tm])

//把map_col的坐标改造成map_bw的坐标 bw to col
#define c2bxy(xxx,yyy) ( -(yyy+move_dy*move_time[move_tm]-height_map_col*move_time[move_tm]) *bw2col_h / move_time[move_tm] ) ,\
						 ( (xxx+move_dx*move_time[move_tm]) * bw2col_w / move_time[move_tm] )
#define c2bx(yyy) -(yyy+move_dy*move_time[move_tm]-height_map_col*move_time[move_tm]) *bw2col_h / move_time[move_tm]
#define c2by(xxx) (xxx+move_dx*move_time[move_tm]) * bw2col_w / move_time[move_tm]


using namespace std;


const string EMPTY_string="---";//不可能自然生成的字符串
const int EMPTY_number=-114514;//不可能自然生成的数字
const char *map_list[]={"..\\map\\Gulang Island.osm","..\\map\\map_big.osm","..\\map\\map_bigger.osm","..\\map\\Fudan.osm","---"};//可选的地图
const char *map_name[]={"鼓浪屿地图（小）","鼓浪屿地图（中）","鼓浪屿地图（大）","五角场地图","自选地图"};//可选的地图的名字
const int N=1000000;//一维数组默认大小
const int M=5000;//地图默认边长
const ll R=64000000;//地球半径
const dbl pi=acos(0)*2;//pi
const ll ban_user[]={6556551,EMPTY_number};//这些个用户上传的路线有误，在读取地图时就不把他们的路线存进来
//const int height_map_bw=2161,width_map_bw=7710;//黑白地图大小
const int height_map_bw=8000,width_map_bw=8000;//黑白地图大小
const int epsx=1,epsy=1;//地图初始放大倍数
const int dx=0,dy=0;//地图初始偏移量
const int fx[24]={-1,-1,-1,0,1,1,1,0,-2,-2,-2,-2,-2,-1,0,1,2,2,2,2,2,1,0,-1},
			fy[24]={1,0,-1,-1,-1,0,1,1,-2,-1,0,1,2,2,2,2,2,1,0,-1,-2,-2,-2,-2};//方向数组
const int height_map_col=800,width_map_col=800;//彩色地图大小
const int bw2col_h=height_map_bw/height_map_col,bw2col_w=width_map_bw/width_map_col;//黑白地图与彩色地图大小的比值
const int move_time[]={0,1,2,4,8,16,32,64,0};//放大倍数，要可以在各个地方整除，所以设置固定倍数。前后2个0为哨兵值

struct Node;
struct Way;
struct Relation;
struct Tuple_Node;
struct Tuple_Way;
struct Edge;//有向边
//typedef union LITTLE;

TiXmlElement *root;
Node *EMPTY_node;//不可能自然生成的node
map<ll,Node *>node;//存储所有Node，可以根据node[x]找到id=x的node
vector<Node *>node_v;//存储所有Node，按id排序。因为是vector，遍历起来比map快。
set<Node *>cross;//所有道路交叉点(定义：在>=2个way中出现过)
set<Node *>door;//区域的出入口(定义：在way和area中均出现过)
set<Tuple_Node>node_xy2id;//(x,y) to *node，根据黑白地图坐标找node的指针
vector<Way>way;//存储所有的道路（不封闭的Way），按id排序
vector<Way>area;//存储所有的区域（封闭的Way），按id排序
vector<Relation>relation;//存储所有的relation，按id排序
vector<Tuple_Way>way_xy2id;//(x1,y1,x2,y2) to *way，根据黑白地图坐标找way的指针
vector<Tuple_Way>area_xy2id;//(x1,y1,x2,y2) to *area，根据黑白地图坐标找area的指针
set<string>tag_choice;//对应的tagv是yes或no的tagk

int load_map=0;//要读取的地图，默认为第一张
dbl minlat,minlon,maxlat,maxlon,dlat,dlon,minlatlon,maxlatlon;
int up_map_bw=height_map_bw , right_map_bw=width_map_bw , down_map_bw=0 , left_map_bw=0;//黑白地图真实范围
int cnt_node_storage;
int map_bw[height_map_bw][width_map_bw];//黑白地图
dbl draw_minlat,draw_minlon,draw_maxlat,draw_maxlon;//黑白地图上的经纬度范围
int cnt_valid_node,cnt_valid_way,cnt_valid_area;//显示在地图上的数目
int move_tm=1,move_dx=0,move_dy=0;//彩色地图框的放大倍数序号 和 移动偏移量
int display_text=0;//是否显示文字
int display_ocean=0;//是否绘制海洋
bool isbreak=0;//是否退出程序
Node *path_endpoint[2];//要查询最短路的路径的端点
int path_cnt;//记录2个路径端点选择的先后顺序
Node *path_point[N];//记录最短路径的所有点
int cnt_path;//最短路径的点数



inline dbl distance(Node *a,Node *b);//两点间地理距离s
inline bool check(int x,int y,int type);//检查坐标是否在地图范围之内，默认黑白地图

inline dbl calc_cross(dbl x1,dbl y1,dbl x2,dbl y2);//计算叉乘
inline dbl calc_dot(dbl x1,dbl y1,dbl x2,dbl y2);//计算点乘
inline dbl calc_distance(dbl x1,dbl y1,dbl x2,dbl y2);//计算平面距离
inline dbl calc_angle(Node *a,Node *b,Node *c);//计算以a为顶点，∠bac的数值（逆时针），返回值0~2Π


inline bool check(int x,int y,int type=0)//检查坐标是否在地图范围之内，默认黑白地图
{
	if(type==0)//黑白地图
		return x>=down_map_bw && x<up_map_bw && y>=left_map_bw && y<right_map_bw;
	if(type==1)//彩色地图
		return x>=0 && x<=width_map_col && y>=0 && y<=height_map_col;
	if(type==2)//输入2个x，判断是否都在彩色地图外的同一侧
		return x<0 && y<0  ||  x>width_map_col && y>width_map_col;
	if(type==3)//输入2个y，判断是否都在彩色地图外的同一侧
		return x<0 && y<0  ||  x>height_map_col && y>height_map_col;
	return 0;
}
