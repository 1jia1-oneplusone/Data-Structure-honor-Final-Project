

inline void draw_dot(int x,int y,int color,int big=1)//在地图数组上画一个点，big决定是否画大点
{
	if(big==0)
		if(check(x,y))map_bw[x][y]=color;
	if(big==1)
		for(int k=0,xx,yy;k<8;k++)
		{
			xx=x+fx[k],yy=y+fy[k];
			if(!check(xx,yy))continue;
			map_bw[xx][yy]=color;
		}
	if(big==2)
		for(int k=0,xx,yy;k<24;k++)
		{
			xx=x+fx[k],yy=y+fy[k];
			if(!check(xx,yy))continue;
			map_bw[xx][yy]=color;
		}
	return;
}

/*
##########################################################################################
#                                                                                        #
#           该段代码参考来源：https://zhuanlan.zhihu.com/p/537899426?utm_id=0            #
#                                                                                        #
##########################################################################################
*/

inline void draw_line(int x0,int y0,int x1,int y1,int color,int big=0)//在地图数组上画直线，Bresenham算法。big决定是否画粗线
{
	bool steep=0;
	if(abs(x1-x0)<abs(y1-y0)) 
	{
		swap(x0,y0);
		swap(x1,y1);
		steep=1;
	}
	if(x0>x1)
	{
		swap(x0, x1);
		swap(y0, y1);
	}
	int dx=x1-x0;
	int dy=y1-y0;
	int deltaY=abs(dy<<1);
	int middle=dx;
	int y=y0;
	for (int x=x0;x<=x1;x++)
	{
		if (steep)
		{
			draw_dot(y,x,color,big);
		}
		else
		{
			draw_dot(x,y,color,big);
		}
		deltaY+=abs(dy<<1);
		if (deltaY>=middle)
		{
			y+=(y1>y0? 1:-1);
			middle+=abs(dx<<1);
		}
	}
	return;
}

//#define debug_cr_ma 
//#define debug_cr_ma2 
inline void create_map_bw()//生成黑白地图数组
{
	for(int i=0;i<height_map_bw;i++)
		for(int j=0;j<width_map_bw;j++)
			map_bw[i][j]=0xffffff;		
	draw_maxlat=draw_maxlon=-10000,draw_minlat=draw_minlon=10000;
	cnt_valid_node=cnt_valid_way=cnt_valid_area=0;
	
	Way wy;
	Node *a,*b;
	
	for(auto x=node.begin() ; x!=node.end() ; x++)//遍历所有note
	{
		a=(*x).second;
			#ifdef debug_cr_ma
					printf("%d,%d ",a->x,a->y);
					if(!check(a->x,a->y))
					{
						printf("!! %.7lf,%.7lf %d,%d\n",a->lat,a->lon,a->x,a->y);
						continue;
					}
			#endif
		if(!check(a->x,a->y))//超出地图边界，就不画了
			continue;
		cnt_valid_node++;
		draw_dot(a->x,a->y,255);
		draw_minlat=min(draw_minlat,a->lat);
		draw_minlon=min(draw_minlon,a->lon);
		draw_maxlat=max(draw_maxlat,a->lat);
		draw_maxlon=max(draw_maxlon,a->lon);
			#ifdef debug_cr_ma2
				if(a->is_isolated())
					a->print();
			#endif
	}
	
	int col[]={0x1335F2,0x32833A,0x420F83,0x2236D8};
	int cnt=0,frog;
	for(auto x=way.begin(); x!=area.end() ; x++,cnt++)//遍历所有way和area
	{
		if(x==way.end())//way遍历完了就切换到area
			x=area.begin();
		wy=*x;
		frog=0;//记录该area/way是否有显示在地图中的部分
		for(int i=0;i<wy.cnt_node;i++)
		{
			a=wy[i];
			b=wy[i-1];
				#ifdef debug_cr_ma
					printf("%d,%d ",a->x,a->y);
					if(!check(a->x,a->y))
					{
						printf("!! %.7lf,%.7lf %d,%d\n",a->lat,a->lon,a->x,a->y);
						continue;
					}
				#endif
			if(!a->display&&!b->display)
				continue;
			if(wy.isarea)
			{
				frog=1;
				draw_line(a->x,a->y,b->x,b->y,0xF22C12,1);
			}
			else 
				if(i)//way不能首尾相连
				{
					frog=1;
					draw_line(a->x,a->y,b->x,b->y,col[cnt%4],1);
				}
		}
		if(wy.isarea)
			cnt_valid_area+=frog;
		else 
			cnt_valid_way+=frog;
	}
	
	for(auto x=node.begin() ; x!=node.end() ; x++)//遍历所有note，对特殊note做出标记
	{
		a=(*x).second;
		if(!check(a->x,a->y))//超出地图边界，就不画了
			continue;
		if(!a->is_normal()||a->is_isolated())
			draw_dot(a->x,a->y,0,2);
	}
	
	return;
}

inline void final_draw()//画框等
{
	
}

/*
##########################################################################################
#                                                                                        #
#     该段代码参考来源：https://blog.csdn.net/zhizhengguan/article/details/106012115     #
#                                                                                        #
##########################################################################################
*/
typedef union {
    uint8_t bytes[4];
    uint32_t value;
}LITTLE;
/*
 * @fileName: bmp file name: test.bmp
 * @width   : bmp pixel width: 32bit
 * @height  : bmp pixel width: 32bit
 * @color   : R[8]/G[8]/B[8]
 * @note    : BMP is l endian mode
 */
inline int bmp_gen_test(char *fileName, uint32_t width, uint32_t height, uint32_t color, int mapp[][width_map_bw])
{
    FILE *fp;
    uint32_t i, j;
    LITTLE l_width, l_height, l_bfSize, l_biSizeImage;

    uint8_t r,g,b;
    uint32_t width_r  =  (width * 24 / 8 + 3) / 4 * 4;
    uint32_t bfSize = width_r * height + 54 + 2;
    uint32_t biSizeImage = width_r * height;

    l_width.value = width;
    l_height.value = height;
    l_bfSize.value = bfSize;
    l_biSizeImage.value = biSizeImage;

    /* BMP file format: www.cnblogs.com/wainiwann/p/7086844.html */
    uint8_t bmp_head_map[54] = {
            /* bmp file header: 14 byte */
            0x42, 0x4d,
            // bmp pixel size: width * height * 3 + 54
            l_bfSize.bytes[0], l_bfSize.bytes[1], l_bfSize.bytes[2], l_bfSize.bytes[3],
            0, 0 , 0, 0,
            54, 0 , 0, 0,    /* 14+40=54 */

            /* bmp map info: 40 byte */
            40, 0, 0, 0,
            //width
            l_width.bytes[0], l_width.bytes[1], l_width.bytes[2], l_width.bytes[3],
            //height
            l_height.bytes[0], l_height.bytes[1], l_height.bytes[2], l_height.bytes[3],
            1, 0,
            24, 00,             /* 24 bit: R[8]/G[8]/B[8] */

            0, 0, 0, 0,     //biCompression:0
//        0, 0, 0, 0,     //biSizeImage锛欰2 00 00 00=162
            l_biSizeImage.bytes[0], l_biSizeImage.bytes[1], l_biSizeImage.bytes[2], l_biSizeImage.bytes[3],
            0, 0, 0, 0,     //biXPelsPerMeter: 60 0F 00 00
            0, 0, 0, 0,     //biYPelsPerMeter
            0, 0, 0, 0,     //biClrUsed
            0, 0, 0, 0      //biClrImportant
    };

    /* write in binary format */
    fp = fopen(fileName, "wb+");
    if(fp == NULL)
    {
        printf("Failed: %s: file create failed!\n", fileName);
        return -1;
    }

    printf("Success: %s file create\n", fileName);

    fwrite(bmp_head_map, sizeof(bmp_head_map), 1, fp);


    for(i = 0; i < height; i++) {
        for(j = 0; j < width; j++)
        {
		    r = mapp[i][j] >> 16;
		    g = mapp[i][j] >> 8;
		    b = mapp[i][j];
            fprintf(fp, "%c%c%c", r,g,b); /* BGR */
		}
        //4 byte align
        for(j = 0; j < width_r-width*3; j++)
            fprintf(fp, "%c", 0);
    }

    fprintf(fp, "%c%c", 0, 0); //PhotoShop two byte "0"

    if(fclose(fp))
    {
        printf("file close failed!\n");
        return -1;
    }
    fp = NULL;

    printf("   width: %d\n", width);
    printf("   height: %d\n", height);
    printf("   R:%d, G:%d, B:%d or #%06x\n", r, g, b, color);

    return 0;
}

inline int draw(int wid,int hei,int mapp[][width_map_bw])
{
    int ret;
    char bmpName[200];

    const char *name = "test";
    uint32_t width = wid;
    uint32_t height = hei;
	uint32_t color = 0x563412;

    /* 生成bmp文件名 */
    sprintf(bmpName, "%s_%d_%d_0x%06x.bmp", name, width, height, color);
    printf("   bmpName: %s\n", bmpName);

    ret = bmp_gen_test(bmpName, width, height, color, mapp);
//    if(!ret)
//        system(bmpName);

    

    return 0;
}

inline void main_draw1(int output=0)
{
	create_map_bw();
	puts("Success: Create black&white map");
	printf("   display range: lat=%.7lf~%.7lf %.7lf ,lon=%.7lf~%.7lf %.7lf\n",draw_minlat,draw_maxlat,draw_maxlat-draw_minlat,draw_minlon,draw_maxlon,draw_maxlon-draw_minlon);
	printf("   valid note=%d/%d, way=%d/%d, area=%d/%d\n\n",cnt_valid_node,node.size(),cnt_valid_way,way.size(),cnt_valid_area,area.size());
	
	if(output)
	{
		draw(width_map_bw,height_map_bw,map_bw);
		puts("Success: Output bmp file");
	}
	return;
}