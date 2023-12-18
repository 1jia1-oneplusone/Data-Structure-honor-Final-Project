//#define debug 
//#define debug_ge_at 
//#define debug_tr_wa 
//#include "common.cpp"
#define hide 

//inline const bool operator <(const Node *x,const Node *y);//重载<运算符，以便排序
//inline const bool operator ==(const Node *x,const Node *y);//重载=运算符，以便二分查找
inline const bool operator <(const set<Node>::iterator &x,const set<Node>::iterator &y);//重载<运算符，以便存放在set中
inline const bool operator <(const Way &x,const Way &y);//重载<运算符，以便排序
//inline const bool operator ==(const Way &x,const Way &y);//重载==运算符，以便调用set中的find函数
inline const char *what_type(const char *c);//看某个属性是什么类型
inline void get_ret(const char *c,string &s);
inline void get_attribute_str(TiXmlElement *nd,const char* att,string &out);// 用于合理得到一个节点nd的属性att，该属性为string类型，存储在out中
template<class T>inline void get_attribute(TiXmlElement *nd,const char* att,T &out);// 用于合理得到一个节点nd的属性att，该属性为type类型(string除外)，存储在out中
inline void get_maxmin();//得到整个文件的最大最小经纬度
inline bool cmp_node_v(Node *a,Node *b);//用于node_v的排序函数
inline void get_all_node();//在set类型node中储存所有点
inline void get_all_way();//在set类型way / area中储存所有点
inline void get_special_node();//寻找cross点和door点
inline void Load_map();

struct Tuple_Node{
	int x,y;
	Node *nd;
};
inline const bool operator <(const Tuple_Node &x,const Tuple_Node &y)//重载<运算符，以便存放在set中
{
	return x.x==y.x? x.y<y.y : x.x<y.x ;
}

struct Node{
	vector<string>tagk,tagv;
	int cnt_tag;
	vector<Edge>edge;
	int cnt_edge;
	ll id,changeset,uid;
	ll belong_way,belong_area;//所处的way、area的id
	int version,visible;
	int display;//是否有显示在初始地图中
	int iscross,isdoor,iswall;//是交叉点，或者不是交叉点
	dbl lat,lon;//纬度、经度
	int x,y;//在黑白地图上的位置
	string timestamp,user;
	TiXmlElement *me;
	
	inline Node(){
		id=changeset=uid=version=visible=lat=lon=belong_way=belong_area=x=y=EMPTY_number;
		iscross=isdoor=iswall=cnt_edge=display=0;
		timestamp=user=EMPTY_string;
		me=NULL;
	}
	inline void init(){
		id=changeset=uid=version=visible=lat=lon=belong_way=belong_area=x=y=EMPTY_number;
		iscross=isdoor=iswall=cnt_edge=display=0;
		timestamp=user=EMPTY_string;
		me=NULL;
		return;
	}
	
	inline bool is_normal()//不是cross、door、wall的普通点
	{
		return !(iscross|isdoor|iswall);
	}
	inline bool is_in_way()//是否处在way中
	{
		return belong_way!=EMPTY_number;
	}
	inline bool is_at_area()//是否处在area边缘
	{
		return is_normal()&&belong_way==EMPTY_number&&belong_area!=EMPTY_number;
	}
	inline bool is_isolated()//是否不属于任何way或area
	{
		return belong_way==EMPTY_number&&belong_area==EMPTY_number;
	}
	
	inline void get_tags()
	{
		TiXmlElement *nd = me->FirstChildElement("tag");//再遍历tag
		string tmp1,tmp2;
		int minn,len;
		for(cnt_tag=0; nd; nd=nd->NextSiblingElement("tag"), cnt_tag++)
		{
			get_attribute_str(nd,"k",tmp1);
			get_attribute_str(nd,"v",tmp2);
/*			if(tmp1=="name"||tmp1=="name:en") //把过长的名字中间插入换行符
			{
				minn=len=tmp2.size();
				if(len>25)
				{
					for(int i=len-1;i>=0;i--)
					{
						if(tmp2[i]==' ' && abs(i-(len>>1))<abs(minn-(len>>1)))
							minn=i;
					}
					tmp2[minn]='n';
					tmp2.insert(minn,"\\");
				}
			}*/
			tagk.push_back(tmp1);
			tagv.push_back(tmp2);
			if(tmp2=="yes"||tmp2=="no")
				tag_choice.insert(tmp1);
		}
	}
	
	inline void get_attributes()
	{
		get_attribute(me,"id",id);
		get_attribute(me,"changeset",changeset);
		get_attribute(me,"uid",uid);
		get_attribute(me,"version",version);
		get_attribute(me,"visible",visible);
		get_attribute(me,"lat",lat);
		get_attribute(me,"lon",lon);
		get_attribute_str(me,"timestamp",timestamp);
		get_attribute_str(me,"user",user);
		get_tags();
		x=((lat-minlat+maxlatlon/2-dlat/2)*height_map_bw/maxlatlon+dx)*epsx;
		y=((lon-minlon+maxlatlon/2-dlon/2)*width_map_bw/maxlatlon+dy)*epsy;
		if(check(x,y))display=1;
		return;
	}
	
	inline const string operator [](const string &x)//用途：node["building"]表示node中tagk="building"对应的tagv，若找不到相应的tagk则返回EMPTY_string
	{
		if(cnt_tag<=5)//点数少，二分反而慢
		{
			for(int i=0;i<cnt_tag;i++)
				if(tagk[i]==x)
					return tagv[i];
		}
		else
		{
			int l=0,r=cnt_tag,mid;
			while(l<r)
			{
				mid=l+r>>1;
				if(tagk[mid]<x)l=mid+1;
				else r=mid;
			}
			if(tagk[l]==x)
				return tagv[l];
		}
		return EMPTY_string;
	}
	
	inline void print()
	{
		if(version<0)return;
		printf("#################################################\
				\n#  id:\t\t\t %lld \t\t#\
				\n#  changeset:\t\t %lld \t\t#\
				\n#  uid:  \t\t %lld   \t\t#\
				\n#  version:\t\t %d \t\t\t#\
				\n#  visible:\t\t %d \t\t\t#\
				\n#  (lat,lon):\t\t (%.7lf,%.7lf) \
				\n#  timestamp:\t\t ",id,changeset,uid,version,visible,lat,lon);
		cout<<timestamp;
		puts("\t#");
		for(int i=0;i<cnt_tag;i++)
		{
			printf("#  ");
			cout<<tagk[i].c_str();
			printf(":\t\t ");
			cout<<tagv[i].c_str()<<endl;
		}
		puts("#################################################\n");
		return;
	}
	
		
};
inline const bool operator <(const Node x,const Node y)//重载<运算符，以便排序
{
	return x.id < y.id;
}
/*
inline const bool operator ==(const Node x,const Node y)//重载==运算符，以便调用set中的find函数
{
	return x.id == y.id;
}*/
inline const bool operator <(const set<Node>::iterator &x,const set<Node>::iterator &y)//重载<运算符，以便存放在set中
{
	return (*x).id<(*y).id;
}

/*
##########################################################################################
#                                                                                        #
#    该段代码参考来源：https://www.zhihu.com/tardis/bd/art/578159391?source_id=1001      #
#                                                                                        #
##########################################################################################
*/
inline dbl distance(Node *a,Node *b)//两点间距离➗地球半径
{
	static dbl t1,t2;
	t1=a->lat*pi/180,t2=b->lat*pi/180;
	return acos(cos(t1)*cos(t2)+sin(t1)*sin(t2)*cos((b->lon-a->lon)*pi/180))*R;
}


struct Tuple_Way{
	int x1,y1,x2,y2;//矩形的左上角，右下角
	vector<Way>::iterator wy;
};
inline const bool operator<(Tuple_Way a,Tuple_Way b)//重载<运算符，以便排序
{
	return a.x1<b.x1;
}
struct Way{
	vector<Node *>mynode;//直接存储指针，便于修改
	vector<string>tagk,tagv;
	ll id,changeset,uid;
	int midx,midy;//中点坐标
	int avgx,avgy;//中心坐标
	int version,visible,isarea,cnt_node,cnt_tag;
	int display;//是否有一部分显示在地图中
	int x1,y1,x2,y2;//矩形的左上角，右下角
	string timestamp,user;
	TiXmlElement *me;
	
	inline Way(){
		id=changeset=uid=version=visible=midx=midy=x1=y1=x2=y2=EMPTY_number;
		timestamp=user=EMPTY_string;
		isarea=cnt_node=cnt_tag=display=x1=x2=y1=y2=0;
		me=NULL;
	}
	
	inline void get_attributes()
	{
		get_attribute(me,"id",id);
		get_attribute(me,"changeset",changeset);
		get_attribute(me,"uid",uid);
		get_attribute(me,"version",version);
		get_attribute(me,"visible",visible);
		get_attribute_str(me,"timestamp",timestamp);
		get_attribute_str(me,"user",user);
		return;
	}
	
	inline void sort()//给tag排个序，按照tagk的大小
	{
		for(int i=0;i<cnt_tag;i++)for(int j=i+1;j<cnt_tag;j++)
			if(tagk[i]>tagk[j])swap(tagk[i],tagk[j]),swap(tagv[i],tagv[j]);
		return;
	}
			
//#define debug_tr_wa 
	inline void travel_way()//遍历一条way的所有东西
	{
		TiXmlElement *nd = me->FirstChildElement("nd");//先遍历node
		ll nd_id;
		Node *tmp;
		midx=midy=0.0;
		x1=y1=100000,x2=y2=-100000;
		
		for(cnt_node=0; nd; nd=nd->NextSiblingElement("nd"), cnt_node++)
		{
			get_attribute(nd,"ref",nd_id);
//#define debug_tr_wa1 
				#ifdef debug_tr_wa1
					printf("[%lld ",nd_id);
//					if(node.find(nd_id))==node.end())puts("warning!");
				#endif
			tmp=node[nd_id]; //通过点的编号找到这一整个点
			
			mynode.push_back(tmp);//加入到mynode中
			
			avgx+=tmp->x,avgy+=tmp->y;
			x1=min(x1,tmp->x),y1=min(y1,tmp->y);
			x2=max(x2,tmp->x),y2=max(y2,tmp->y);
			
				#ifdef debug_tr_wa1
					printf("%lld ]",(*mynode[cnt_node]).id);
				#endif
				#ifdef debug_tr_wa
					(*tmp).print();
				#endif
				
			//以下为特殊way的处理
			if(id==164669117)//超出海岸线的建筑
				if(nd_id==1223211447)
					while(nd_id!=1223211196)
						nd=nd->NextSiblingElement("nd"),
						get_attribute(nd,"ref",nd_id);
		}
		
		avgx/=cnt_node,avgy/=cnt_node;
		midx=(mynode[cnt_node>>1]->x+mynode[cnt_node+1>>1]->x)>>1,midy=(mynode[cnt_node>>1]->y+mynode[cnt_node+1>>1]->y)>>1;
		
		isarea=((*(mynode[0])).id==(*(mynode[cnt_node-1])).id);//判断是否为封闭道路，即区域area
		
		nd = me->FirstChildElement("tag");//再遍历tag
		string tmp1,tmp2;
		int minn,len;
		for(cnt_tag=0; nd; nd=nd->NextSiblingElement("tag"), cnt_tag++)
		{
			get_attribute_str(nd,"k",tmp1);
			get_attribute_str(nd,"v",tmp2);
/*			if(tmp1=="name"||tmp1=="name:en") //把过长的名字中间插入换行符
			{
				minn=len=tmp2.size();
				if(len>25)
				{
					for(int i=len-1;i>=0;i--)
					{
						if(tmp2[i]==' ' && abs(i-(len>>1))<abs(minn-(len>>1)))
							minn=i;
					}
					tmp2[minn]='n';
					tmp2.insert(minn,"\\");
				}
			}*/
			tagk.push_back(tmp1);
			tagv.push_back(tmp2);
				#ifdef debug_tr_wa
					printf("tagk=");
					cout<<tmp1<<endl;
				#endif
			if(tmp2=="yes"||tmp2=="no")
				tag_choice.insert(tmp1);
		}
		
		sort();
		
			#ifdef debug_tr_wa
				puts("Finished: travel_way");
			#endif
		return;
	}
	
	inline void print()
	{
		printf("#################################################\
				\n#  id:\t\t %lld \t\t\t#\
				\n#  changeset:\t %lld \t\t\t#\
				\n#  uid:  \t %lld   \t\t\t#\
				\n#  version:\t %d \t\t\t\t#\
				\n#  visible:\t %d \t\t\t\t#\
				\n#  node number:\t %d \t\t\t\t#\
				\n#  timestamp:\t ",id,changeset,uid,version,visible,cnt_node);
		cout<<timestamp;
		puts("\t\t#");
		if(cnt_node<=5)for(int i=0;i<cnt_node;i++)printf("%lld ",(*mynode[i]).id);
		puts("\n#################################################\n");
		return;
	}
	
	inline const string operator [](const string &x)//用途：way["building"]表示way中tagk="building"对应的tagv，若找不到相应的tagk则返回EMPTY_string
	{
		if(cnt_tag<=5)//点数少，二分反而慢
		{
			for(int i=0;i<cnt_tag;i++)
				if(tagk[i]==x)
					return tagv[i];
		}
		else
		{
			int l=0,r=cnt_tag,mid;
			while(l<r)
			{
				mid=l+r>>1;
				if(tagk[mid]<x)l=mid+1;
				else r=mid;
			}
			if(tagk[l]==x)
				return tagv[l];
		}
		return EMPTY_string;
	}
	
	inline Node* operator [](const int &x)//用途：way[2]表示way中第2个点
	{
		if(x>=0)return mynode[x];
		else return mynode[cnt_node+x];
	}
	
};
inline const bool operator <(const Way &x,const Way &y)//重载<运算符，以便存放在set中
{
	return x.id < y.id;
}
inline const bool operator ==(const Way &x,const Way &y)//重载==运算符，以便调用set中的find函数
{
	return x.id == y.id;
}


struct Relation{
	vector<Node *>mynode;//存储指针
	vector<string>mn_role;//mynode的role
	vector<int>myway;//存储下标
	vector<bool>mw_isarea;//myway的isarea
	vector<string>mw_role;//myway的role
	vector<string>tagk,tagv;
	ll id,changeset,uid;
	int midx,midy;//中点坐标
	int avgx,avgy;//中心坐标
	int version,visible,isarea,cnt_member,cnt_tag;
	int display;//是否有一部分显示在地图中
	int x1,y1,x2,y2;//矩形的左上角，右下角
	string timestamp,user;
	TiXmlElement *me;
	
	inline Relation(){
		id=changeset=uid=version=visible=midx=midy=x1=y1=x2=y2=EMPTY_number;
		timestamp=user=EMPTY_string;
		isarea=cnt_member=cnt_tag=display=x1=x2=y1=y2=0;
		me=NULL;
	}
	
	inline void get_attributes()
	{
		get_attribute(me,"id",id);
		get_attribute(me,"changeset",changeset);
		get_attribute(me,"uid",uid);
		get_attribute(me,"version",version);
		get_attribute(me,"visible",visible);
		get_attribute_str(me,"timestamp",timestamp);
		get_attribute_str(me,"user",user);
		return;
	}
	
	inline void sort()//给tag排个序，按照tagk的大小
	{
		for(int i=0;i<cnt_tag;i++)for(int j=i+1;j<cnt_tag;j++)
			if(tagk[i]>tagk[j])swap(tagk[i],tagk[j]),swap(tagv[i],tagv[j]);
		return;
	}
			
//#define debug_tr_wa 
	inline void travel_relation()//遍历一个relation的所有东西
	{
		TiXmlElement *nd = me->FirstChildElement("member");//先遍历member
		ll idd;
		string type,role;
		Node *tmp;
		Way tmpp2;
		midx=midy=0.0;
		x1=y1=100000,x2=y2=-100000;
		
		for(cnt_member=0; nd; nd=nd->NextSiblingElement("member"), cnt_member++)
		{
			get_attribute(nd,"ref",idd);
			get_attribute_str(nd,"type",type);
			get_attribute_str(nd,"role",role);
			
			if(type=="node")
			{
				if(node.find(idd)==node.end())continue;//不存在
				tmp=node[idd]; //通过点的编号找到这一整个点
				
				mynode.push_back(tmp);//加入到mynode中
				mn_role.push_back(role);
				
				avgx+=tmp->x,avgy+=tmp->y;
				x1=min(x1,tmp->x),y1=min(y1,tmp->y);
				x2=max(x2,tmp->x),y2=max(y2,tmp->y);
				
			}
			else if(type=="way")
			{
				tmpp2.id=idd;
				auto tmpp=lower_bound(way.begin(),way.end(),tmpp2);//通过way的编号找到它
				if(tmpp==way.end())//不是way就是area
				{
					tmpp=lower_bound(area.begin(),area.end(),tmpp2);
					if(tmpp==area.end())continue;//不存在
					mw_isarea.push_back(1);
					myway.push_back(tmpp-area.begin());
				}
				else 
				{
					mw_isarea.push_back(0);
					myway.push_back(tmpp-way.begin());
				}
				
				mw_role.push_back(role);
				
				tmpp2=*tmpp;
				avgx+=tmpp2.avgx,avgy+=tmpp2.avgy;
				x1=min(x1,tmpp2.x1),y1=min(y1,tmpp2.y1);
				x2=max(x2,tmpp2.x2),y2=max(y2,tmpp2.y2);
				
			}
		}
		
		avgx/=cnt_member,avgy/=cnt_member;
		
		nd = me->FirstChildElement("tag");//再遍历tag
		string tmp1,tmp2;
		int minn,len;
		for(cnt_tag=0; nd; nd=nd->NextSiblingElement("tag"), cnt_tag++)
		{
			get_attribute_str(nd,"k",tmp1);
			get_attribute_str(nd,"v",tmp2);
/*			if(tmp1=="name"||tmp1=="name:en") //把过长的名字中间插入换行符
			{
				minn=len=tmp2.size();
				if(len>25)
				{
					for(int i=len-1;i>=0;i--)
					{
						if(tmp2[i]==' ' && abs(i-(len>>1))<abs(minn-(len>>1)))
							minn=i;
					}
					tmp2[minn]='n';
					tmp2.insert(minn,"\\");
				}
			}*/
			tagk.push_back(tmp1);
			tagv.push_back(tmp2);
			
			if(tmp2=="yes"||tmp2=="no")
				tag_choice.insert(tmp1);
		}
		
		sort();
		
			#ifdef debug_tr_wa
				puts("Finished: travel_way");
			#endif
		return;
	}
	
	inline void print()
	{
		printf("#################################################\
				\n#  id:\t\t %lld \t\t\t#\
				\n#  changeset:\t %lld \t\t\t#\
				\n#  uid:  \t %lld   \t\t\t#\
				\n#  version:\t %d \t\t\t\t#\
				\n#  visible:\t %d \t\t\t\t#\
				\n#  member number:\t %d \t\t\t\t#\
				\n#  timestamp:\t ",id,changeset,uid,version,visible,cnt_member);
		cout<<timestamp;
		puts("\t\t#");
		for(int i=0;i<cnt_tag;i++)printf("%s",tagv[i]);
		puts("\n#################################################\n");
		return;
	}
	
	inline const string operator [](const string &x)//用途：way["building"]表示way中tagk="building"对应的tagv，若找不到相应的tagk则返回EMPTY_string
	{
		if(cnt_tag<=5)//点数少，二分反而慢
		{
			for(int i=0;i<cnt_tag;i++)
				if(tagk[i]==x)
					return tagv[i];
		}
		else
		{
			int l=0,r=cnt_tag,mid;
			while(l<r)
			{
				mid=l+r>>1;
				if(tagk[mid]<x)l=mid+1;
				else r=mid;
			}
			if(tagk[l]==x)
				return tagv[l];
		}
		return EMPTY_string;
	}
	
	
};
inline const bool operator <(const Relation &x,const Relation &y)//重载<运算符，以便存放在set中
{
	return x.id < y.id;
}
inline const bool operator ==(const Relation &x,const Relation &y)//重载==运算符，以便调用set中的find函数
{
	return x.id == y.id;
}


inline const char *what_type(const char *c)//看某个属性是什么类型
{
	if(c=="long long"||c=="id"||c=="changeset"||c=="uid"||c=="ref")return "long long";
	if(c=="bool"||c=="visible")return "bool";
	if(c=="int"||c=="version")return "int";
	if(c=="double"||c=="lat"||c=="lon"||c=="maxlat"||c=="minlat"||c=="maxlon"||c=="minlon")return "double";
	return "string";
}


inline void get_ret(const char *c,string &s)
{
	if(c==NULL)s=EMPTY_string;
	else s=c;
	return;
}


// ↓ 用于合理得到一个节点nd的属性att，该属性为string类型，存储在out中
inline void get_attribute_str(TiXmlElement *nd,const char* att,string &out)
{
	get_ret(nd->Attribute(att),out);
	return;
}


// ↓ 用于合理得到一个节点nd的属性att，该属性为type类型(string除外)，存储在out中
template<class T>inline void get_attribute(TiXmlElement *nd,const char* att,T &out)
{
	const char *type=what_type(att);
		#ifdef debug_ge_at
			printf("%s's type = %s\n",att,type);
		#endif
	const char *s=nd->Attribute(att);
	if(s==NULL)
	{
		out=EMPTY_number;
		return;
	}
	
	int i,frog,mul;
	
	if(type=="bool")
	{
		out=(s[0]=='1'||s[0]=='t');
			#ifdef debug_ge_at
				printf("bool %s %s %d\n",s,"true",s[0]=='t');
			#endif
	}
	if(type=="int"||type=="long long")
	{
		out=0LL;
		for(i=0;s[i];i++)out=out*10+s[i]-'0';
	}
	if(type=="double")
	{
		out=0.0;
		for(i=0,frog=0,mul=1;s[i];frog=((s[i]=='.')|frog),frog&&(mul*=frog*10),i++)
			if(s[i]!='.')out=out*10+s[i]-'0';
		out/=mul/10;
	}
	return;
}

//#define debug_ge_ma 
inline void get_maxmin()//得到整个文件的最大最小经纬度
{
	TiXmlElement *nd = root->FirstChildElement("bounds");
	get_attribute(nd,"minlat",minlat);
	get_attribute(nd,"maxlat",maxlat);
	get_attribute(nd,"minlon",minlon);
	get_attribute(nd,"maxlon",maxlon);
		#ifdef debug_ge_ma
			minlat+=0.05;maxlat+=0.03;
			minlon+=0.15;maxlon+=0.03;
		#endif
	dlat=maxlat-minlat,dlon=maxlon-minlon;
	minlatlon=min(dlat,dlon);
	maxlatlon=max(dlat,dlon);
	
	return;
}

inline bool cmp_node_v(Node *a,Node *b)//用于node_v的排序函数
{
	return a->id<b->id;
}

//#define debug_ge_al_no 
inline void get_all_node()//储存所有点
{
	//分配空间
	TiXmlElement *nd = root->FirstChildElement("node");
	for(cnt_node_storage=0; nd; nd=nd->NextSiblingElement("node"), cnt_node_storage++, cnt_node++);
	node_storage=(Node *)malloc(sizeof(Node)*(cnt_node_storage+10000));
	
	nd = root->FirstChildElement("node");
	EMPTY_node=node_storage;//创造空节点
	
	Node *tmp;
	for(int cnt=1; nd; nd=nd->NextSiblingElement("node"), cnt++)
	{
		tmp=node_storage+cnt;
		tmp->init();
		tmp->me=nd;
		tmp->get_attributes();
		node[tmp->id]=tmp;
		node_v.push_back(tmp);
		node_xy2id.insert((Tuple_Node){tmp->x,tmp->y,tmp});
			#ifdef debug_ge_al_no
//				(*tmp).print();
				printf("%d,%d ",tmp->x,tmp->y);
			#endif
	}
	sort(node_v.begin(),node_v.end(),cmp_node_v);
	return;
}

//#define debug_ge_al_wa 
//#define debug_ge_al_wa_nooo
inline void get_all_way()//在vector类型way / area中储存所有点
{
	TiXmlElement *wy = root->FirstChildElement("way");
		#ifdef debug_ge_al_wa
			int count=0;
		#endif
	Way *tmp;
	Way wyy;
	
	way.push_back(wyy);//垫一个哨兵节点
	
	bool ban=0;
	for(int cnt=0; wy; wy=wy->NextSiblingElement("way"), cnt++)
	{
		tmp=new Way;//错误点：如果没有每次new一个空间，则set中所有的元素的mycode全部共用同一个。
		tmp->me=wy;
		tmp->get_attributes();
		
		ban=0;
		for(int i=0;ban_user[i]!=EMPTY_number;i++)//检查该用户是否被ban
			if(tmp->uid==ban_user[i])
				ban=1;
		for(int i=0;ban_way[i]!=EMPTY_number;i++)//检查该路线是否被ban
			if(tmp->id==ban_way[i])
				ban=1;
		if(ban)continue;
		
		tmp->travel_way();
			#ifdef debug_ge_al_wa
				printf("%lld ",(*tmp).cnt_node);
				if(cnt>1360)(*tmp).print();
			#endif
			
		
		if(tmp->isarea)
			area.push_back((*tmp));
		else 
		{
			way.push_back(*tmp);
			#ifdef debug_ge_al_wa
//				printf("%d ",count++);
//				tmp.print();
			#endif
		}
		
	}
	
	//按id排序
	auto x=way.begin();x++;
	sort(way.begin(),way.end());
	sort(area.begin(),area.end());
	
	
	for(auto x=way.begin(); x!=way.end() ; x++)//遍历所有way
	{
		wyy=*x;
		way_xy2id.push_back((Tuple_Way){wyy.x1,wyy.y1,wyy.x2,wyy.y2,x});
	}
	
	for(auto x=area.begin(); x!=area.end() ; x++)//遍历所有area
	{
		wyy=*x;
		area_xy2id.push_back((Tuple_Way){wyy.x1,wyy.y1,wyy.x2,wyy.y2,x});
	}
	
	sort(way_xy2id.begin(),way_xy2id.end());
	sort(area_xy2id.begin(),area_xy2id.end());
	
	return;
}

inline void get_all_relation()//存储所有relation
{
	Relation x;
	TiXmlElement *rl = root->FirstChildElement("relation");
	Relation *tmp;
	Relation wyy;
	bool ban=0;
	for(int cnt=0; rl; rl=rl->NextSiblingElement("relation"), cnt++)
	{
		tmp=new Relation;
		tmp->me=rl;
		tmp->get_attributes();
		
		ban=0;
		for(int i=0;ban_user[i]!=EMPTY_number;i++)//检查该用户是否被ban
			if(tmp->uid==ban_user[i])
				ban=1;
		if(ban)continue;
		
		tmp->travel_relation();
		
		relation.push_back((*tmp));
		
	}
	
	//按id排序
	sort(relation.begin(),relation.end());
	
	return;
}

//#define debug_ge_sp_no 
inline void get_special_node()//寻找cross点和door点
{
	Way yy;
	Node *y;
	map<ll,int>vis;
	for(auto x=way.begin() ; x!=way.end() ; x++)//遍历所有way，寻找cross
	{
		if((*x).id==EMPTY_number)continue;
		
		yy=*x;
		for(int i=0;i<yy.cnt_node;i++)
		{
			y=yy.mynode[i];
			if(y->belong_way==EMPTY_number)
			//if(vis.find(y.id)==vis.end())
			{
				y->belong_way=yy.id;
					#ifdef debug_ge_sp_no
						vis[y->id]=1;
					#endif
			}
			else
			{
				y->iscross=1;//被2条way拥有，是cross
				cross.insert(y);
					#ifdef debug_ge_sp_no
						vis[y->id]++;
					#endif
			}
		}
	}
	for(auto x=area.begin() ; x!=area.end() ; x++)//遍历所有area，寻找door和wall
	{
		yy=*x;
		for(int i=0;i<yy.cnt_node;i++)
		{
			y=yy.mynode[i];
			if(y->belong_area==EMPTY_number)
			{
				y->belong_area=yy.id;
				if(y->belong_way!=EMPTY_number)//被一个way和一个area拥有，是door
					y->isdoor=1;
					#ifdef debug_ge_sp_no
						if(vis.find(y->id)==vis.end())vis[y->id]=100;
						else vis[y->id]+=100;
					#endif
			}
			else if(y->belong_area!=yy.id)
			{
				y->iswall=1;//被2个area拥有，是wall
				door.insert(y);
					#ifdef debug_ge_sp_no
						vis[y->id]+=100;
					#endif
			}
		}
	}
		#ifdef debug_ge_sp_no
			for(auto x=vis.begin(); x!=vis.end();x++)
			{
//				if(1<(*x).second&&(*x).second<100||(*x).second>101)printf("#%lld %d  ",(*x).first,(*x).second);
				if((*x).second%100>1)printf("#%lld %d  ",(*x).first,(*x).second);
			}
		#endif
		
	return;
}


inline void Load_map()
{
	TiXmlDocument tinyXmlDoc(map_list[load_map]);	
	tinyXmlDoc.LoadFile();
	puts("√ 加载文件完成");
	
	root = tinyXmlDoc.RootElement();
	
	get_maxmin();
	printf("√ 获取地图经纬度范围 (lat=%.7lf~%.7lf %.7lf lon=%.7lf~%.7lf %.7lf)\n",minlat,maxlat,maxlat-minlat,minlon,maxlon,maxlon-minlon);
		
	get_all_node();
	printf("√ 读取所有node (总数: %d)\n",node.size());
	
	get_all_way();
	printf("√ 读取所有way和area (总数: way=%d , area=%d)\n",way.size(),area.size());
	
	get_all_relation();
	printf("√ 读取所有relation (总数: %d)\n",relation.size());
	
	get_special_node();
	printf("√ 获取所有特殊点 (总数: cross=%d , door=%d)\n",cross.size(),door.size());
	
//#define debug1	
		#ifdef debug1
			puts("All choice tag:");
			for(auto x=tag_choice.begin() ; x!=tag_choice.end() ; x++)
			{
				cout<<*x<<endl;
			}
		#endif
		
//#define debug2	
		#ifdef debug2
/*			puts("All node:");
			for(auto x=node.begin() ; x!=node.end() ; x++)
			{
				cout<<(*x).id<<' ';
			}
			cout<<endl;/
			
/*			Node tmp;
			tmp.id=11292616182;
			tmp=*node.find(tmp);
			printf("%lld\n",tmp.id);*/
			
/*			puts("All area:");
			int tot[100];
			for(int i=0;i<100;i++)tot[i]=0;
			Way y;
			for(auto x=area.begin() ; x!=area.end() ; x++)
			{
				y=*x;
				tot[y.cnt_tag]++;
				//if((*x).cnt_tag==1)cout<<(*x).tagk[0]<<'-'<<(*x)[(*x).tagk[0]]<<' ';
				cout<<y["building"];
			}
			for(int i=0;i<100;i++)if(tot[i])printf("%d %d\n",i,tot[i]);*/
			
			puts("All ways' begins and ends:");//各个道路之间，A的起点几乎不会是B的终点
			Way y,z;
			int tot=0;
//			int tot[100000];
//			for(int i=0;i<100000;i++)tot[i]=0;
			dbl eps=0.0000000000000001;
			printf("in eps=%.10lf\n",eps);
			map<ll,int>vis;
			for(auto x=way.begin() ; x!=way.end() ; x++)
			{
				y=*x;
				tot=0;
				for(auto xx=way.begin(); xx!=x; xx++)
				{
					z=*xx;
					if(distance(y[0],z[0])<eps||distance(y[0],z[-1])<eps
						||distance(y[-1],z[0])<eps||distance(y[-1],z[-1])<eps)
						tot++;
				}
				printf("%d ",tot);
//				tot[y[0].id%100000]++;tot[y[1].id%100000]++;
			}
//			for(int i=0;i<100000;i++)if(tot[i])printf("%d ",tot[i]);
			
		#endif
		
//#define display1 
		#ifdef display1
			for(int i=0;i<area.size();i++)
			{
				if(area[i]["natural"]!=EMPTY_string)
				{
					cout<<area[i]["natural"]<<' ';
				}
			}
		#endif
		
//#define display2 
		#ifdef display2
			for(int i=0;i<cnt_node;i++)
				printf("%lld ",node_storage[i].belong_way);
		#endif
	return;
}

/*
choice tag:
area
bridge
building
bus
covered
ferry
foot
historic
internet_access:fee
oneway
takeaway
tunnel
wheelchair
*/