
inline dbl calc_cross(dbl x1,dbl y1,dbl x2,dbl y2)//计算叉乘
{
	return x1*y2-x2*y1;
}

inline dbl calc_dot(dbl x1,dbl y1,dbl x2,dbl y2)//计算点乘
{
	return x1*x2+y1*y2;
}

inline dbl calc_distance(dbl x1,dbl y1,dbl x2,dbl y2)//计算平面距离
{
	return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

inline dbl calc_angle(Node *a,Node *b,Node *c)//计算以a为顶点，∠bac的数值（逆时针），返回值0~2pi
{
//								printf("[ %d,%d %d,%d %d,%d ]  ",a->x,a->y,b->x,b->y,c->x,c->y);
	dbl pro=calc_distance(a->x,a->y,b->x,b->y)*calc_distance(a->x,a->y,c->x,c->y);
	dbl sinn=calc_cross(b->x-a->x , b->y-a->y , c->x-a->x , c->y-a->y)/pro;//计算sin值
	dbl coss=calc_dot(b->x-a->x , b->y-a->y , c->x-a->x , c->y-a->y)/pro;//计算cos值
	
	if(sinn<-1)sinn=-1;if(sinn>1)sinn=1;//由于精度不够，所以需要加入边界特判
	if(coss<-1)coss=-1;if(coss>1)coss=1;
	
	if(sinn>=0&&coss>=0)return asin(sinn);
	if(sinn>=0&&coss<=0)return pi-asin(sinn);
	if(sinn<=0&&coss<=0)return pi-asin(sinn);
	if(sinn<=0&&coss>=0)return pi*2+asin(sinn);
}