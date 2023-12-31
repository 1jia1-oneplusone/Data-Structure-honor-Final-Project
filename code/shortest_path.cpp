
struct Edge{//有向边
	ll val;//边长（地理距离）
	Node *from,*to;//端点
	vector<Way>::iterator belong;//这条边是哪个way或area的
};

struct sp_{//shortest_path() 的优先队列用到的结构体，需要封装一大堆东西
	ll dis; //距离
	Node *last,*now;
	int from; //from=1/2 从path_endpoint[0]/从path_endpoint[1]访问
	vector<Way>::iterator belong;//这条边是哪个way或area的
	
};
inline const bool operator <(const sp_ &x,const sp_ &y)//重载<运算符，以便存放在priority_queue中
{
	return x.dis < y.dis;
}

inline void add_edge(Node *from, Node *to, vector<Way>::iterator belong, int type=2)//建单/双向边，默认双向
{
	ll val=distance(from,to);
	from->edge.push_back((Edge){val,from,to,belong}),from->cnt_edge++;
	if(type==2)
		to->edge.push_back((Edge){val,to,from,belong}),to->cnt_edge++;
	return;
}

inline void build_graph()//建立图
{
	Way wy;
	Node *ndi,*ndj;
	int cnt=0;
	for(auto x=way.begin(); x!=area.end(); x++,cnt++)//遍历每一条way和area
	{
		if(x==way.end())//way遍历完了就切换到area
			x=area.begin();
		wy=*x;
		for(int i=1;i<wy.cnt_node;i++)//在node之间建边
		{
			add_edge(wy[i-1],wy[i],x);
		}
	}
	
	puts("√ 建立图");
	
	return;
}

/*inline int shortest_path()//查询2点间最短路,a*双向搜索
{
	priority_queue<sp_>q;
	Node *x,*y,*v,*ans,*ans2;
	sp_ sp;
	map<Node*,int>vis;//记录点是否访问过，1表示从path_endpoint[0]访问，2表示从path_endpoint[1]访问
	map<Node*,Node*>last;//记录每个点的上一个点
	map<Node*,ll>dis[2];//记录每个点离出发点的最短距离
	int ok=0,home,cnt=0;
	ll minn=0x7ffffffffffffff,maxx=0;
	ll distanc=0LL;//整段路程的长度，不命名为distance是因为已经有一个同名函数了
	
	if(path_endpoint[0]==EMPTY_node || path_endpoint[1]==EMPTY_node)//没有选中2个点
	{
		puts("× 请在查询路径前选择2个点。确保地图上有2个靶子图案（不要求显示在屏幕中）。");
		return 0;
	}
	
	//初始化
	for(int i=0;i<=1;i+=1)//把2个端点扔进优先队列
		q.push((sp_){0LL,path_endpoint[i],path_endpoint[i],i+1}),
		dis[i][path_endpoint[i]]=0;
	
	
	while(q.size())
	{
		cnt++;
		sp=q.top();q.pop();
		x=sp.now;y=sp.last;home=sp.from;
		sp.dis=-sp.dis;
			#ifdef debug_sh_pa
				setlinewidth(2);
				setcolor(BLACK);
				line(b2cxy(x->x,x->y), b2cxy(y->x,y->y));
				printf("%dth, id=%lld, from %d %lld\n",cnt,x->id,home-1,dis[home>>1][x]);
			#endif
		
		if(dis[home>>1][x]>minn*2)//已经不会有更短的路程了
		{
				#ifdef debug_sh_pa
					puts("byebye");
				#endif
			break;
		}
		
		if(vis[x]>0 || cnt>=1000000)//如果这个点被访问过了，或者已经执行太久了
		{
			if(vis[x]==home || vis[x]==3)//如果是从同一个起点出发，访问了2次，就跳过
				continue;
			else//否则x是从2个起点分别访问到，那么就找到一个解了
			{
				vis[x]=3;
				ok=1;
				maxx=max(maxx,dis[0][x]+dis[1][x]);
				
				if(minn>dis[0][x]+dis[1][x])//一个更优的解
				{
					minn=dis[0][x]+dis[1][x];
					ans=x;
					ans2=y;
				}
				
				if(minn*2<=maxx) //已经不会有更短的路程了
					break;
				
			}
		}
		vis[x]=home;//标记访问
		last[x]=y;
		
		for(int i=0,tmp;i<x->cnt_edge;i++)//遍历所有相邻点，注意不是访问
		{
			v=x->edge[i].to;
			if(v==y)continue;
			if((*(x->edge[i].belong)).isarea)continue;//不能走area的边
			
			tmp=vis[v];
			if(tmp==home || (dis[home>>1].find(v)==dis[home>>1].end()? 0 : dis[home>>1][v]<sp.dis+x->edge[i].val) )//如果该点访问过了，或者最短距离更短，就跳过
				continue;
			
			dis[home>>1][v]=sp.dis+x->edge[i].val;
			
				#ifdef debug_sh_pa
					setlinewidth(3);
					setcolor(BLUE);
					line(b2cxy(x->x,x->y), b2cxy(v->x,v->y));
					printf("%lld ",v->id);
				#endif
				
			q.push((sp_){-(sp.dis+x->edge[i].val),x,v,home});
		}
	}
	
	if(!ok)//没找到
	{
		puts("两点间没有相连路径或者相距太远，请选择其它点进行查询。");
		return 0;
	}
	
	
		#ifdef debug_sh_pa
			puts("haha!!");
		#endif
	
	cnt_path=0;
	x=ans;y=ans2;
	
	for(v=y;last[v]!=v;cnt_path++,v=last[v])//先存一段
	{
		path_point[cnt_path]=v;
	}
	path_point[cnt_path++]=v;
	for(int i=0;i<cnt_path/2;i++)
	{
		swap(path_point[i],path_point[cnt_path-i-1]);
	}
	
	for(v=x;last[v]!=v;cnt_path++,v=last[v])//再存另一段
	{
		path_point[cnt_path]=v;
	}
	path_point[cnt_path]=v;
	
		#ifdef debug_sh_pa
			for(int i=0;i<=cnt_path;i++)printf("%lld ",path_point[i]->id);
			puts("");
		#endif
	
	distanc=dis[0][x]+dis[1][x];
	printf("两点间最短路径长度为：%lldm\n",distanc);
	return 1;
}*/

//#define debug_sh_pa 
inline int shortest_path()//查询2点间最短路,a*单向搜索
{
	if(path_endpoint[0]==EMPTY_node || path_endpoint[1]==EMPTY_node)//没有选中2个点
	{
		puts("× 请在查询路径前选择2个点。确保地图上有2个靶子图案（不要求显示在屏幕中）。");
		return 0;
	}
	
	//看看2个端点是不是原图不存在的点或者不在way上
	for(int i=0;i<=1;i++)
	{
		if(path_endpoint[i]->version!=-1 && path_endpoint[i]->is_in_way())continue;
		
		Tuple_Node tmp,tmpp;
		tmp.nd=tmpp.nd=EMPTY_node;
		set<Tuple_Node>::iterator tmpn;
		
		int flag=(path_endpoint[i]->version==-1);
		
		tmp.x=path_endpoint[i]->x;
		tmp.y=path_endpoint[i]->y;
			
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
		
		if(flag)
		{
			add_edge(path_endpoint[i], tmp.nd, way.begin());
			path_endpoint[i]->version=0;
			cnt_node_storage+=2;
		}
		else
		{
			add_edge(path_endpoint[i], tmp.nd, way.begin());
		}
		
		
	}
	
	
	priority_queue<sp_>q;
	ll w;
	Node *x,*y,*v,*ans,*ans2;
	sp_ sp;
	Edge e;
	map<Node*,int>vis;//记录点是否访问过，1表示从path_endpoint[0]访问，2表示从path_endpoint[1]访问
	map<Node*,Node*>last;//记录每个点的上一个点
	map<Node*,ll>dis;//记录每个点离出发点的最短距离（带权）
	map<Node*,ll>dis_real;//记录每个点离出发点的最短距离
	int ok=0;
	cnt_path=0;
	
	//初始化
	q.push((sp_){0LL,EMPTY_node,path_endpoint[0],0}),
	dis[path_endpoint[0]]=0;
	dis_real[path_endpoint[0]]=0;
	
	while(q.size())
	{
		sp=q.top();q.pop();
		x=sp.now,y=sp.last;
		
		if(vis[x])continue;
		
		last[x]=y;
		
		if(x==path_endpoint[1])//找到了
		{
			ok=1;
			break;
		}
		
		for(int i=0;i<x->cnt_edge;i++)
		{
			e=x->edge[i];
			v=e.to;
			w=e.val;
			if(display_bus && (*e.belong).isbus)//如果是公交线路，而且用户想走公交线路，就降低公交线路的代价
				w*=0.3;
			if(vis[v])continue;
			if(dis.find(v)!=dis.end()&&dis[x]+w>=dis[v])
				continue;
				
			dis[v]=dis[x]+w;
			dis_real[v]=dis_real[x]+e.val;
			sp.last=x, sp.now=v, sp.dis=-dis[v], sp.belong=e.belong;
			q.push(sp);
		}
		
	}
	
	
	if(!ok)//没找到
	{
		puts("两点间在地图中没有相连路径或者相距太远，请选择其它点进行查询。");
		if(path_endpoint[0]->is_at_area() || path_endpoint[1]->is_at_area())
			puts("请注意不要误选到建筑上的点，因为你没法沿着建筑外墙行走。");
		return 0;
	}
		
	
	for(v=path_endpoint[1];v!=EMPTY_node;cnt_path++,v=last[v])//存下路径
	{
		path_point[cnt_path]=v;
	}
	
		#ifdef debug_sh_pa
			for(int i=0;i<cnt_path;i++)printf("%lld ",path_point[i]->id);
			puts("");
		#endif
	
	printf("两点间最短路径长度为：%lf公里\n",dis_real[path_endpoint[1]]/1000.0);
	
	return 1;
}