
inline void ui_bar()//窗口栏绘制
{
	color_t fc=getfillcolor(),c=getcolor();
	
	setfillcolor(EGERGB(208,220,232));
	setcolor(EGERGB(198,207,216));
	setlinewidth(2);
	
	bar(0,height_map_col,width_map_col,height_map_col+window_bar);
	line(0,height_map_col,width_map_col,height_map_col);
	
	setfillcolor(fc);setcolor(c);
}

inline void ui_line(int x1,int y1,int x2,int y2,PIMAGE pic=NULL,int col=-1,int thickness=-1)//用ege画线
{
	if(thickness>=0)
		setlinewidth(thickness);
	if(col>=0)
		setcolor(EGEARGB(0xFF, col>>16, col>>8, col));
	line(b2cxy(x1,y1), b2cxy(x2,y2),pic);
	return;
}

//wood stone beach bare_rock coastline water
inline void ui_natural(Way wy,PIMAGE pic=NULL)//画自然景观（tagk="natural"）
{
	static int frog=0;
	static Node *a;
	static ege_point tmp[20000];
	static string k;
	k=wy["natural"];
	if(k=="beach"||k=="sand") setfillcolor(EGEARGB(0xFF, 250,250,232));
	else if(k=="water") setfillcolor(EGEARGB(0xFF, 173,213,254));
	else if(k=="wood"||wy["landuse"]=="grass") setfillcolor(EGEARGB(0xFF, 32,193,123));
	else if(k=="bare_rock"||k=="stone"||k=="coastline") setfillcolor(EGEARGB(0xFF, 153,153,153));
	else setfillcolor(RED);
	
	for(int i=0;i<wy.cnt_node;i++)
	{
		a=wy[i];
		if(check(b2cxy(a->x,a->y),1))frog=1;
		tmp[i]=(ege_point){b2cxy((a->x),(a->y))};
	}
	if(frog)//一个地图区域只要有一个点在地图范围内，就要整个都画，否则填充会出问题。
	{
		ege_fillpoly(wy.cnt_node,tmp,pic);//画一团多边形色块
	}
	
	setfillcolor(EGEARGB(0xFF, 233,240,245));
	return;
}

//#define debug_ge_is 
inline void get_island(Node *a,Node *b,PIMAGE pic=NULL)//从给定2点开始寻找并绘制岛屿（封闭曲线），类似凸包算法
{
	Node *beg=a,*v,*choose;
	ege_point tmp[200000];
	int cnt=0;
	dbl max_angle,tmp2,last_angle;
	Way tmpp;
	
	for(cnt=0; b!=beg ; cnt++,a=b,b=choose)
	{
		tmp[cnt]=(ege_point){b2cxy(a->x,a->y)};//先把上个点加上去
		
			#ifdef debug_ge_is
				printf("\n#%lld %d# ",b->id,cnt);
				settextjustify(CENTER_TEXT, CENTER_TEXT);
				setfont(10, 0, "宋体");
				setcolor(BLACK);
//				xyprintf(b2cxy(b->x, b->y), "%lld", cnt);
			#endif
		
		max_angle=-1000;
		for(int i=0;i<b->cnt_edge;i++)//寻找从上一条边开始，逆时针方向第一条边
		{
			v=b->edge[i].to;
			tmpp=*(b->edge[i].belong);
			if(v==a && b->cnt_edge>1 || tmpp["route"]=="ferry" || tmpp["power"]=="line" || tmpp["waterway"]=="river")
				continue;//(1)一般不会再访问上一个点，除非这个点是死胡同，只能掉头了 (2)不要走航线等特殊路线
			
			tmp2=calc_angle(b,a,v);//计算夹角
			
				#ifdef debug_ge_is
//					printf("%lld,%lf ",v->id,tmp2/pi*180);
				#endif
			
			if(tmp2 > max_angle)
			{
				max_angle=tmp2;
				choose=v;
			}
		}
		
	}
	
	setfillcolor(EGEARGB(0xFF, 197,240,211));
	ege_fillpoly(cnt-1,tmp,pic);
	
	/*setcolor(BLACK);
	setlinewidth(6);
	ege_drawpoly(cnt-1,tmp,pic);*/
	
	return;
}

//#define debug_is 
inline void island(PIMAGE pic=NULL)
{
	ege_point tmp[20000];
	int cnt;
	get_island(node[1763041878],node[1763041608],pic);//绘制鼓浪屿岛的陆地，这两个点是从岛屿边缘任选的连续2个点
	get_island(node[2398387661],node[2398387607],pic);//厦门岛
	get_island(node[4239351927],node[63089182],pic);//大屿岛
	get_island(node[7077375465],node[7077375425],pic);//海沧岛
	get_island(node[5789784263],node[7663037403],pic);//漳州岛
	Way wy;
	Relation rl;
	
/*	for(int i=0;i<relation.size();i++)//以relation形式存在的岛屿
	{
		rl=relation[i];
		if(rl["place"]=="islet"||rl["place"]=="island")
		{
			cnt=0;
			for(int k=0;k<rl.mw_isarea.size();k++)
			{
				wy=way[rl.myway[k]];
				for(int j=0; j<wy.cnt_node; j++)
					tmp[cnt++]=(ege_point){b2cxy(wy[j]->x,wy[j]->y)};
			}
			setfillcolor(EGEARGB(0xFF, 153,153,153));
			ege_fillpoly(cnt,tmp);
		}
	}*/
	
	
	//厦门岛
	wy.id=334467999;
	wy=*lower_bound(area.begin(),area.end(),wy);
		#ifdef debug_is
			wy.print();
		#endif
	for(int i=0; i<wy.cnt_node; i++)
	{
		tmp[i]=(ege_point){b2cxy(wy[i]->x,wy[i]->y)};
			#ifdef debug_is
				printf("%d ",i);
			#endif
	}
	setfillcolor(EGEARGB(0xFF, 197,240,211));
	ege_fillpoly(wy.cnt_node-1,tmp,pic);
		#ifdef debug_is 
			puts("ok");
		#endif
	return;
}

inline void ui_way(Way wy,int cnt,PIMAGE pic=NULL,int coll=0)//画way,coll表示需不需要调整画笔
{
	static int col[]={CYAN,DARKGRAY,LIGHTBLUE,MAGENTA,0xFFFFFF};//way显示的颜色
	
	if(!coll)
	{
		if(wy["route"]=="ferry")//航线
			setlinestyle(USERBIT_LINE,(WORD)0x000F);
		else if(wy["boundary"]=="administrative")//行政区域划分，由于在地图数据中不完整，因此干脆直接不显示
			return;
		else if(wy["natural"]=="coastline")//海岸线
			setlinestyle(SOLID_LINE),setlinewidth(2),setcolor(BLACK);
		else//默认值
		{
			setlinestyle(SOLID_LINE);
			setlinewidth(2-(move_tm==1));
			setcolor(EGEARGB(0xFF,col[cnt%4]>>16, col[cnt%4]>>8, col[cnt%4]));
		}
	}
	
	//--------以上为特殊way----------
	
	Node *a,*b;
	for(int i=1;i<wy.cnt_node;i++)
	{
		a=wy[i];
		b=wy[i-1];
		ui_line(a->x,a->y,b->x,b->y,pic);
	}
	return;
}

inline void ui_area(Way wy,PIMAGE pic=NULL,int coll=0)//画area,coll表示需不需要调整画笔
{
	if(!coll)
	{
		if(wy["cly"]=="no") //已经单独画过的区域
			return;
		else if(wy["natural"]!=EMPTY_string || wy["landuse"]=="grass") //自然景观
		{
			ui_natural(wy,pic);
			return;
		}
		else if(wy["landuse"]=="residential") //小区，画了反而会覆盖里面包含的建筑
			return;
		else if(wy["seamark:name"]!=EMPTY_string) //莫名其妙的海域
			return;
		else//默认值
		{
			setlinewidth(1);
			setcolor(EGEARGB(0xFF, 18,44,242));
			setfillcolor(EGEARGB(0xFF, 233,240,245));
		}
	}
	
	//--------以上为特殊area----------
	
//	frog=0;
	Node *a;
	ege_point tmp[200];
	for(int i=0;i<wy.cnt_node;i++)
	{
		a=wy[i];
//		if(check(b2cxy(a->x,a->y),1))frog=1;
		tmp[i]=(ege_point){b2cxy((a->x),(a->y))};
	}
	
	ege_fillpoly(wy.cnt_node,tmp,pic);//画一团多边形色块
		
	if(wy["building"]!=EMPTY_string || wy["leisure"]!=EMPTY_string || wy["office"]!=EMPTY_string || wy["amenity"]!=EMPTY_string 
			|| wy["landuse"]=="military" || wy["landuse"]=="construction" || wy["shop"]!=EMPTY_string)//描边
		ege_drawpoly(wy.cnt_node,tmp,pic);
	
	return;
}

//#define debug_ui1 
//#define debug_ui1_fast 
inline void ui1()//绘制界面
{
	
	if(display_ocean)
		setbkcolor(EGERGB(152,209,252));	//设置背景色为浅蓝（海的颜色）
	else
		setbkcolor(WHITE);
	cleardevice();
	setlinewidth(2);//设置线宽
	
	Way wy;
	Node *a,*b;
	Node nd;
	Relation rl;
	
	int cnt=0,frog,thickness=0;
	
	
	island();//绘制岛屿	
	
	ege_point tmp[200];
	
	static int col[]={CYAN,DARKGRAY,LIGHTBLUE,MAGENTA,0xFFFFFF};//way显示的颜色
	const char *text;//显示的字符
	LPCSTR font;//字体
	int font_size;
	
			
	setlinewidth(1);
	setcolor(EGEARGB(0xFF, 18,44,242));
	setfillcolor(EGEARGB(0xFF, 233,240,245));
	
/*	for(auto x=area.begin(); x!=way.end() ; x++,cnt++)//遍历所有way和area
	{
		if(x==area.end())//area遍历完了就切换到way
		{
			x=way.begin();
			cnt=0;
		}
		wy=*x;*/
	
	Tuple_Way find;
	find.x1=c2bx(height_map_bw);
	
		#ifdef debug_ui1_fast
			for(auto x=way_xy2id.begin(); x!=way_xy2id.end() ; x++)
			{
				auto y=*((*x).wy);
				printf("%d ",y.x1);
			}
			printf("#%d#\n",find.x1);
			Way xx;xx.id=231881183;
			auto x=*lower_bound(way.begin(),way.end(),xx);
			printf("%d,%d ",x.x1,x.y1);
			printf("%d",find.x1);
		#endif
	
	
	for(auto x=lower_bound(area_xy2id.begin(),area_xy2id.end(),find); x!=way_xy2id.end() ; x++,cnt++)//绘制所有way和area，用一点点优化
	{
		if(x==area_xy2id.end())//area遍历完了就切换到way
		{
				#ifdef debug_ui1_fast
					puts("&&");
				#endif
			x=lower_bound(way_xy2id.begin(),way_xy2id.end(),find);
			cnt=0;
			if(x==way_xy2id.end())break;
		}
		wy=*((*x).wy);
			#ifdef debug_ui1_fast
				//printf("#%d %d  ",cnt,wy.x1);
				if(wy.id==231881183)puts("!!");
			#endif
		
/*		if(!wy.isarea)//设置颜色
			setcolor(EGEARGB(0xFF, col[cnt%4]>>16, col[cnt%4]>>8, col[cnt%4]));*/
		
		if(check(b2cx(wy.y1),b2cx(wy.y2),2)||check(b2cy(wy.x1),b2cy(wy.x2),3))//如果整个都在地图范围外面，就不画了
			continue;
			
		if(wy.isarea) //area 绘制填充多边形
		{			
			ui_area(wy);
		}
		
		else //way 一段一段画直线
		{
			ui_way(wy,x-way_xy2id.begin());
		}
		
	}
	
	
	//绘制relation的公交线路
	if(display_bus)
	{
		setlinewidth(2*move_time[move_tm]/move_tm);
		setcolor(EGERGB(176,149,105));
		for(int ii=0;ii<relation.size();ii++)
		{
			rl=relation[ii];
			
				frog=0;
				for(int i=0;i<rl.cnt_tag;i++)
				{
					if(rl.tagv[i]=="bus")frog=1;
				}
				if(!frog)continue;
				
				//rl.print();
				
			for(int i=0;i<rl.mynode.size();i++)
			{
				a=rl.mynode[i];
				circle(b2cxy(a->x,a->y),10*move_time[move_tm]/(move_tm+1));
			}
			for(int i=0;i<rl.myway.size();i++)
			{
				if(rl.mw_isarea[i]==0)
					ui_way(way[rl.myway[i]],0,NULL,1);
				else
					ui_area(area[rl.myway[i]],NULL,1);
			}
		}
		setcolor(BLACK);
	}
	
	
	//显示选中点，画一个靶子
	setlinewidth(2);
	setcolor(BLACK);
	for(int i=0;i<=1;i++)
	{
		if(path_endpoint[i]==EMPTY_node)
			continue;
			
		a=path_endpoint[i];
		circle(b2cxy(a->x,a->y),10*move_time[move_tm]/move_tm);
		ege_line(b2cx(a->y)-15*move_time[move_tm]/move_tm ,b2cy(a->x), b2cx(a->y)+15*move_time[move_tm]/move_tm, b2cy(a->x));
		ege_line(b2cx(a->y), b2cy(a->x)-15*move_time[move_tm]/move_tm, b2cx(a->y), b2cy(a->x)+15*move_time[move_tm]/move_tm);
		
	}
	
	
	//绘制最短路径
	setlinestyle(SOLID_LINE);
	setlinewidth(6*move_time[move_tm]/move_tm);
	setcolor(LIGHTRED);
	for(int i=1;i<cnt_path;i++)
	{
		a=path_point[i-1];b=path_point[i];
		ui_line(a->x,a->y,b->x,b->y);
	}
	
	
	if(minlatlon<=move_time[move_tm]*0.005 && display_text)//显示文字（放大到一定倍数才显示）
	{
		find.x1=c2bx(height_map_bw);
		cnt=0;
		for(auto x=lower_bound(area_xy2id.begin(),area_xy2id.end(),find); x!=way_xy2id.end() ; x++,cnt++)//遍历way和area，显示文字
		{
			if(x==area_xy2id.end())//area遍历完了就切换到way
			{
					#ifdef debug_ui1_fast
						puts("&&");
					#endif
				x=lower_bound(way_xy2id.begin(),way_xy2id.end(),find);
				setcolor(EGEARGB(0xFF, 18,44,242));
				cnt=0;
				if(x==way_xy2id.end())break;
			}
			wy=*((*x).wy);
			
			if(wy["name"]!=EMPTY_string||wy["name:en"]!=EMPTY_string)
			{
				settextjustify(CENTER_TEXT, CENTER_TEXT);
				setbkmode(TRANSPARENT);//文字背景透明
				
				if(!wy.isarea)//way的文字与对应的way同色
					setcolor(EGEARGB(0xFF,col[cnt%4]>>16, col[cnt%4]>>8, col[cnt%4]));
				
				/*if(wy["name"]!=EMPTY_string)
					text=wy["name"].c_str(),
					font="宋体",
					font_size=10;
				else //没有中文名，只好退而求其次显示英文名*/
					text=wy["name:en"].c_str(),
					font="Calibri",
					font_size=12;

//#define debug_ui11 
					#ifdef debug_ui11
						cout<<'^'<<wy["name"];
						printf("%s ",text);
					#endif
				
				setfont(font_size*move_time[move_tm]/move_tm, 0, font, 0, 0, FW_BOLD, 0, 0, 0);
				
				if(wy.isarea) xyprintf( b2cxy(wy.avgx, wy.avgy), "%s", text);//area就输出在中心
				else xyprintf( b2cxy(wy.midx, wy.midy), "%s", text);//way就输出在中点
			}
		}
		
		
		cnt=0;
		for(auto x=node_v.begin() ; x!=node_v.end() ; x++,cnt++)//遍历点
		{
			if(!check(a->x,a->y))//超出地图边界，就不画了
				continue;
			
			nd=*((*x));
			
				#ifdef debug_ui1 //这里的调试用来输出地图上特定点的id，以此来在地图文件中找到每一个特殊note/way的真实身份
					//设置文字样式，输出文字
	/*				if(a->id==1223212343)
					{
						settextjustify(CENTER_TEXT, CENTER_TEXT);
						setfont(10, 0, "宋体");
						setcolor(BLACK);
						xyprintf(b2cxy(a->x, a->y), "%lld", a->id);
					}
					
					if(a->id==8168609858)
					{
						putpixel(b2cxy(a->x,a->y),EGEARGB(0xFF, 0,0,0));
						circle(b2cxy(a->x,a->y),5);
					}*/
				#endif
			
			if(a->is_isolated())//孤立点一般有很多tag
			{
				//do something
			}
			
			if(nd["name"]!=EMPTY_string||nd["name:en"]!=EMPTY_string)
			{
				settextjustify(CENTER_TEXT, CENTER_TEXT);
				setbkmode(TRANSPARENT);//文字背景透明
				
				if(!wy.isarea)//与对应的way同色
					setcolor(EGEARGB(0xFF,col[cnt%4]>>16, col[cnt%4]>>8, col[cnt%4]));
				
				if(nd["name"]!=EMPTY_string)
					text=nd["name"].c_str(),
					font="宋体",
					font_size=10;
				else //没有中文名，只好退而求其次显示英文名
					text=nd["name:en"].c_str(),
					font="Calibri",
					font_size=12;
				
				setfont(font_size*move_time[move_tm]/move_tm, 0, font, 0, 0, FW_BOLD, 0, 0, 0);
				
				xyprintf( b2cxy(nd.x, nd.y), "%s", text);
			}
		}
	}
	
	ui_bar();//窗口栏
	
//	puts("Success: Generate UI.");
//	getch();

	
	return;
}

inline void fix_mdxy()//修正move_dx和move_dy，不至于超出地图范围
{
	move_dx=max(move_dx,0);
	move_dx=min(move_dx, width_map_col - width_map_col/move_time[move_tm] );
	move_dy=max(move_dy,0);
	move_dy=min(move_dy, height_map_col - height_map_col/move_time[move_tm] );
	return;
}

//#define debug_ro 
inline int roll(mouse_msg msg)//滚轮调整地图大小，返回1/0 是否有实际调整地图大小
{
	if(msg.wheel==0)
		return 0;
	
	int xc=msg.x,yc=msg.y;//滚轮时鼠标的位置
	
	//计算彩色地图框的参数，具体演算过程见草稿纸
	//原则：滚轮时鼠标所在点 在地图上的位置不变
	if(msg.wheel>0&&move_time[move_tm+1])//放大
	{
		move_tm++;
		move_dx+=(move_time[move_tm] - move_time[move_tm-1]) * xc / move_time[move_tm] / move_time[move_tm-1];
		move_dy+=(move_time[move_tm] - move_time[move_tm-1]) * yc / move_time[move_tm] / move_time[move_tm-1];
			#ifdef debug_ro
				printf("%d,%d x%d\n",move_dx,move_dy,move_time[move_tm]);
			#endif
		fix_mdxy();
		return 1;
	}
	if(msg.wheel<0&&move_time[move_tm-1])//缩小
	{
		move_tm--;
		move_dx-=(move_time[move_tm+1] - move_time[move_tm]) * xc / move_time[move_tm] / move_time[move_tm+1];
		move_dy-=(move_time[move_tm+1] - move_time[move_tm]) * yc / move_time[move_tm] / move_time[move_tm+1];
			#ifdef debug_ro
				printf("%d,%d x%d\n",move_dx,move_dy,move_time[move_tm]);
			#endif
		fix_mdxy();
		return 1;
	}
	
	return 0;
}

inline int drag(int &x1,int &y1,int x2,int y2)//鼠标左键拖动地图位置，返回1/0 是否有实际拖动地图位置
{
	int tmp1=move_dx,tmp2=move_dy;
	move_dx+=(x1-x2)/move_time[move_tm];//等比例移动
	move_dy+=(y1-y2)/move_time[move_tm];
	fix_mdxy();
	if(tmp1!=move_dx)x1=x2;
	if(tmp2!=move_dy)y1=y2;
	return tmp1!=move_dx||tmp2!=move_dy;//如果和一开始的dxdy一样，就不要再渲染了
}

inline int move(int x,int y)//方向键改变地图位置，以坐标形式输入。返回1/0 是否有实际改变地图位置
{
	move_dx+=x*2;//移动
	move_dy+=y*2;
	fix_mdxy();
	return move_dx-x!=move_dx||move_dy-y!=move_dy;//如果和一开始的dxdy一样，就不要再渲染了
}

#define debug_cl 
inline int click(mouse_msg msg,int bad_choose=0)//鼠标单击左键，选择该处的点。bad_choose:如果选的点不在node中，是否要强行找一个最近的
{
		#ifdef debug_cl
			puts("click!");
			if(bad_choose)puts("strong!");
		#endif
	static Tuple_Node tmp,tmpp;
	tmp.nd=tmpp.nd=EMPTY_node;
	static set<Tuple_Node>::iterator tmpn;
	static int frog=0;
	static Node *p;
	ll min_distance=0x7fffffffffffffff;
	
	tmp.x=c2bx(msg.y);//获取鼠标位置
	tmp.y=c2by(msg.x);
	
	
	for(int k=0;k<=17;k++)//在周围一圈找node，曼哈顿距离由小到大搜索
	{
		for(int i=-k;i<=k;i++)
		{
			for(int j=abs(i)-k;j<=k-abs(i);j++)
			{
				tmpp.x=tmp.x+i;
				tmpp.y=tmp.y+j;
				tmpn=node_xy2id.find(tmpp);
				
				if(tmpn!=node_xy2id.end())//找到了
				{
					tmp=*tmpn;
					if(!tmp.nd->is_isolated())
						i=j=k=1000;//退出循环
				}
			}
		}
	}
	
	if(tmpn==node_xy2id.end()||!tmp.nd->is_in_way())//没找到或者在建筑上
	{
		Node *Tmp=tmp.nd;
		int flag=(tmpn==node_xy2id.end());
		if(!bad_choose&&flag)
		{
			puts("附近没有可选点，请重新选点，或按ctrl+左键来强行选择该处点。");
			return 0;
		}	puts("1");
		
		if(flag)
		{
			//创建一个新点
			cnt_node_storage++;
			Node nd;
			nd.init();
			nd.id=-cnt_node_storage;
			nd.x=tmp.x;
			nd.y=tmp.y;
			nd.lat=(dbl)(nd.x/epsx-dx)*maxlatlon/height_map_bw+(dbl)minlat-maxlatlon/2+dlat/2;
			nd.lon=(dbl)(nd.y/epsy-dy)*maxlatlon/width_map_bw+(dbl)minlon-maxlatlon/2+dlon/2;
			nd.version=-1;
			node_storage[cnt_node_storage]=nd;
		}
		
		for(int k=0;k<=10000;k++)//在周围一圈扩大范围继续找node，曼哈顿距离由小到大搜索
		{
			for(int i=-k;i<=k;i++)
			{
				for(int j=abs(i)-k;j<=k-abs(i);j++)
				{
					tmpp.x=tmp.x+i;
					tmpp.y=tmp.y+j;
					tmpn=node_xy2id.find(tmpp);
					
					if(tmpn!=node_xy2id.end())//找到了
					{
						tmp=*tmpn;
						if(tmp.nd->is_in_way())
							i=j=k=10000000;//退出循环
					}
				}
			}
		}
			puts("2");
			if(tmpn==node_xy2id.end())puts("Fail!!");
			else printf("new %lld\n",tmp.nd->id);
		
		if(flag)
		{
			add_edge(&node_storage[cnt_node_storage], tmp.nd, way.begin());
			tmp.nd=&node_storage[cnt_node_storage];
		}
		if(!flag)
		{
			add_edge(Tmp, tmp.nd, way.begin());
										printf("area %lld ",tmp.nd->id);
			tmp.nd=Tmp;
		}
	}
	else 
	{
		puts("选中点的信息：");
		tmp.nd->print();
	}
	
		cnt_path=0;//清空当前最短路径
		frog=1;
		for(int i=0;i<=1;i++)
		{
			if(path_endpoint[i]==tmp.nd)//如果重复选择一个node，就取消它，并且不影响另一个选择的node
				path_endpoint[i]=EMPTY_node,
				path_cnt=i^1,
				frog=0;
		}
		if(frog)//否则就把它记录下来，且覆盖掉最开始选的点
			path_cnt^=1,
			path_endpoint[path_cnt]=tmp.nd;
			
	return 1;//需要重新渲染
	
/*	else
	{
		cout<<"附近没有可选点"<<endl;
	}
	return 0;*/
}

#define debug_co 
inline int control()//处理全部鼠标信息 返回0：啥也不干 返回1：退出程序
{
	static PIMAGE png=newimage();//存储图片文件
	static int redraw=0;//表示需要何种程度的渲染
	static int notclick=0,isdrag=0;
	static int clear=1;//是否需要清空控制台
	static int x1,y1,x2,y2;
	static ll cnt,cnt2;
	static auto lasttime=chrono::system_clock::now(),nowtime=lasttime;//记录时间
		
	static key_msg key;
	static mouse_msg msg;
	
	
	lasttime=nowtime,nowtime=chrono::system_clock::now();//更新时间
	
	while(mousemsg() || kbmsg())
	{
		redraw=0;
		notclick=0;
		
		lasttime=nowtime,nowtime=chrono::system_clock::now();//更新时间				
		
		if(mousemsg())
		{
			msg=getmouse();
				
			if(msg.is_move())//鼠标移动
			{
				if(isdrag)//如果是左键按下(即拖动)状态
				{
						#ifdef debug_co
//							printf("%lld,%lld ",cnt,(nowtime-lasttime).count()/100000);
						#endif
						
					x2=msg.x,y2=msg.y;
					if(cnt%7==0 && drag(x1,y1,x2,y2))//如果动了就渲染。添加cnt的条件是为了手动降帧，防止短时间大量渲染导致更加卡顿
					{
						ui1();
						cnt+=5040;
						#ifdef debug_co
//							printf("move ");
						#endif
					}
				}
			}
					
			else if(msg.is_left() && msg.is_down())//左键按下，拖动
			{
					#ifdef debug_co
//						printf("left pressed!\n");
					#endif
				x1=x2=msg.x;//记下当前鼠标位置
				y1=y2=msg.y;
				isdrag=1;//标记为按下
				cnt=0;//计时器
			}
			
			else if(msg.is_left() && msg.is_up())//左键抬起，单击
			{
				if(cnt<=1)//如果鼠标按下左键后动了 或者 一秒后才抬起，那么就不要判定为单击
				{
					clear=!cnt_path;
					if(click(msg,iscontrol))
					{
						ui1();
					}
					clear^=!cnt_path;
				}
					#ifdef debug_co
//					else
//						printf("not click!\n");
					#endif
				isdrag=0;//重置拖动状态
				cnt=0;
					#ifdef debug_co
//						printf("%d ",isdrag);
					#endif
			}
					
			else if(msg.is_right() && msg.is_down())//右键按下，中止
			{
				isbreak=1;
				return 1;
			}
			
			else if(msg.is_mid() && msg.is_down())//中键按下，截图
			{
				getimage(png,0,0,width_map_col,height_map_col+window_bar);//获取屏幕图像
				if(savepng(png, "Screenshot.png"))//输出.png文件，失败返回1
					puts("保存截图失败");
				else
					puts("√ 成功保存截图Screenshot.png至当前目录");
				
				isdrag=0;//重置拖动状态
				cnt=0;
			}
					
			else if(msg.is_wheel())//滚动，缩放地图
			{
				if(roll(msg))
					ui1();
				isdrag=0;//重置拖动状态
				cnt=0;
			}
			
		
			if(clear && !msg.is_move())//清空控制台
			{
				clear=0;
				system("cls");
			}
		}
		else if(kbmsg())
		{
			//for(int i=0;i<=2;i++)key=getkey();//按任何键都会连续发送3条相同的消息，因此提前消掉
			key=getkey();
			
			if(key.key==key_esc && key.msg==key_msg_down)//esc，退出
			{
				isbreak=1;
				return 1;
			}
			
			if(0x25<=key.key && key.key<=0x28 && key.msg==key_msg_down)//左上右下，移动地图
			{
				if(move(fx[(key.key-0x25)*2+1],fy[(key.key-0x25)*2+1]))
					ui1();
					#ifdef debug_co
//						puts("move!");
					#endif
			}
			
			if(key.key==key_enter && key.msg==key_msg_down)//enter，计算最短路径
			{
				if(shortest_path())
					ui1();
			}
			
			if(key.key==key_space && key.msg==key_msg_down)//空格，显示文字
			{
				display_text^=1;
				ui1();
			}
			
			if(key.key==key_control && key.msg==key_msg_down)//按下/松开control
			{
				iscontrol^=1;
				ui1();
			}
			
			
			if(clear)//清空控制台
			{
				clear=0;
				system("cls");
			}
		}
		
		if(isdrag)cnt++;//拖动的计时器
		
		Sleep(1);
	}
	
	return 0;
}

inline void judge_ocean()//判断是否绘制海洋
{
	for(auto x=area.begin(); x!=way.end() ; x++)//遍历所有way和area
	{
		if(x==area.end())//area遍历完了就切换到way
			x=way.begin();
		if((*x)["place"]=="islet"||(*x)["place"]=="island")
			display_ocean=1;
	}
	return;
}


inline void choose_map()//选择要加载的地图
{
	setbkcolor(EGERGB(152,209,252));	//设置背景色为浅蓝（海的颜色）
	setcolor(BLACK);
	
	settextjustify(CENTER_TEXT,CENTER_TEXT);//居中
	setfont(30, 0, "宋体", 0, 0, FW_BOLD, 0, 0, 0);
	setbkmode(TRANSPARENT);//文字背景透明
	
	//left top right bottom depth topflag
	int bottom[][6]={{65,250,365,400,10,1},{425,250,725,400,10,1},{65,450,365,600,10,1},{425,450,725,600,10,1},{250,650,550,750,10,1},{-1,0,0,0,0,0}};
	
	ui_bar();//窗口栏
	
	outtextxy(400,150,"请选择你要加载的地图");
	
	setfillcolor(0xA8A8A8);//LIGHTGRAY
	for(int i=0;bottom[i][0]!=-1;i++)
	{
		bar3d(bottom[i][0],bottom[i][1],bottom[i][2],bottom[i][3],bottom[i][4],bottom[i][5]);
		outtextxy((bottom[i][0]+bottom[i][2])/2,(bottom[i][1]+bottom[i][3])/2,map_name[i]);
//		outtextrect(bottom[i][0],bottom[i][1],(bottom[i][2]-bottom[i][0]),(bottom[i][3]-bottom[i][1]),map_name[i]);
//		outtextrect((bottom[i][0]+bottom[i][2])/2,(bottom[i][1]+bottom[i][3])/2,(bottom[i][2]-bottom[i][0])/2,(bottom[i][3]-bottom[i][1])/2,map_name[i]);
	}
	
	int x=-1,y=-1,x1=-1,y1=-1,pick=-1,quit=0;
	mouse_msg msg=getmouse();
	delay_ms(1);
	//getch();
	
	while(is_run()&&!isbreak&&!quit)
	{
		while(mousemsg() && !quit)
		{
			msg=getmouse();
			
			if(msg.is_move())//移动
			{
				if(pick>=0)
				{
					x=msg.x;y=msg.y;//记下当前鼠标位置
					if(!(x>=bottom[pick][0] && y>=bottom[pick][1] && x<=bottom[pick][2] && y<=bottom[pick][3]))//移出按钮范围了
					{
						setcolor(getbkcolor());
						setfillcolor(EGERGB(152,209,252));
						bar3d(bottom[pick][0]-bottom[pick][4],bottom[pick][1]-bottom[pick][4],bottom[pick][2]+bottom[pick][4],bottom[pick][3]+bottom[pick][4],0,0);//涂掉原来的按钮
					
						setcolor(BLACK);
						setfillcolor(0xA8A8A8);
						bar3d(bottom[pick][0],bottom[pick][1],bottom[pick][2],bottom[pick][3],bottom[pick][4],bottom[pick][5]);
						outtextxy((bottom[pick][0]+bottom[pick][2])/2,(bottom[pick][1]+bottom[pick][3])/2,map_name[pick]);//画新的
						
						pick=-1;
					}
				}
			}
			else if(msg.is_left() && msg.is_down())//左键按下
			{
				x=msg.x;y=msg.y;//记下当前鼠标位置
				pick=-1;
				for(int i=0;bottom[i][0]!=-1;i++)
				{
					if(x>=bottom[i][0] && y>=bottom[i][1] && x<=bottom[i][2] && y<=bottom[i][3])
						pick=i;
				}
				if(pick>=0)
				{
					setcolor(getbkcolor());
					setfillcolor(getbkcolor());
					bar3d(bottom[pick][0]-bottom[pick][4],bottom[pick][1]-bottom[pick][4],bottom[pick][2]+bottom[pick][4],bottom[pick][3]+bottom[pick][4],0,0);//涂掉原来的按钮
					
					setcolor(BLACK);
					setfillcolor(EGERGB(127,127,127));
					bar3d(bottom[pick][0]+bottom[pick][4]/2,bottom[pick][1]-bottom[pick][4]/2,bottom[pick][2]+bottom[pick][4]/2,bottom[pick][3]-bottom[pick][4]/2,bottom[pick][4]/2,bottom[pick][5]);
					outtextxy((bottom[pick][0]+bottom[pick][2]+bottom[pick][4])/2,(bottom[pick][1]+bottom[pick][3]-bottom[pick][4])/2,map_name[pick]);
					
				}
			}
			
			else if(msg.is_left() && msg.is_up())//左键抬起
			{
				if(pick>=0)
				{
					if(x>=bottom[pick][0] && y>=bottom[pick][1] && x<=bottom[pick][2] && y<=bottom[pick][3])
					{
						load_map=pick;
						quit=1;
					}
				
					setcolor(getbkcolor());
					setfillcolor(getbkcolor());
					bar3d(bottom[pick][0]-bottom[pick][4],bottom[pick][1]-bottom[pick][4],bottom[pick][2]+bottom[pick][4],bottom[pick][3]+bottom[pick][4],0,0);//涂掉原来的按钮
				
					setcolor(BLACK);
					setfillcolor(0xA8A8A8);
					bar3d(bottom[pick][0],bottom[pick][1],bottom[pick][2],bottom[pick][3],bottom[pick][4],bottom[pick][5]);
					outtextxy((bottom[pick][0]+bottom[pick][2])/2,(bottom[pick][1]+bottom[pick][3])/2,map_name[pick]);//画新的
						
				}
			}
					
			else if(msg.is_right() && msg.is_down())//右键按下，中止
			{
				isbreak=1;
				break;
			}
			delay_ms(1);
		}
	}
	
	if(map_name[pick]=="自选地图")
	{
		setcolor(LIGHTGRAY);
		setfillcolor(DARKGRAY);
		bar3d(50,300,750,500,0,0);
		setcolor(WHITE);
		setfont(35, 0, "宋体", 0, 0, FW_BOLD, 0, 0, 0);
		outtextxy(400,400,"请点击控制台输入地图文件路径或相对路径");
		
		printf("请输入地图文件路径，或相对main.exe的路径，输入后请按enter\n");
		
		delay_ms(1);
		char ch[100000];int cnt=0;
		char c=getchar();
		while(c>31&&c<127)
		{
			ch[cnt++]=c;
			c=getchar();
		}
		map_list[pick]=ch;
	}
	
	return;
}

inline void main_ui()
{
	path_endpoint[0]=path_endpoint[1]=EMPTY_node;
	path_cnt=0;
	
	judge_ocean();
	
	//whole_map=newimage(6400,6400);
	ui1();//先画出最初的地图
	
	puts("√ 绘制UI");
	
	puts("---------------------------");
	puts("初始化全部完成，在地图上进行任意动作以清除以上消息。");
	
	bool opt;
	
	while(is_run()&&!isbreak)
	{
		opt=control();
	}
	
	return;
}

inline void ui_initialize()
{
	initgraph(width_map_col, height_map_col+window_bar, INIT_RENDERMANUAL);//创建窗口
	
	setrendermode(RENDER_MANUAL);//设置为手动渲染模式，以后绘制完成后，需要使用getche()等等待类函数才会显示新画的东西
	ege_enable_aa(true);//开启抗锯齿(默认为不开启)	
	
	return;
}