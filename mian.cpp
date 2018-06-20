
    /**	@file       mian.cpp
     *	@note       Hikvision Digital Technology Co., Ltd. All Right Reserved.
     *	@brief
     *
     *	@author     lipengfei
     *	@date       2018/05/10
     *	@note       历史记录：
     *	@note       V1.0.0
     *	@warning
     */

    #include <stdlib.h>
    #include <stdio.h>
    #include "OSSocket.h"
    #include "JsonParse.h"
    #include "CmdParse.h"
    #include <memory>
    #include <iostream>
    #include "vector"
    #include <set>
    #include "time.h"
    #include <math.h>
    #include "omp.h"
    #include <algorithm>
    #include <map>
//#include <chrono>

    using  namespace std;
    #define MAX_SOCKET_BUFFER       (1024 * 1024 * 4)       /// 发送接受数据最大4M
    int goods_tag[MAX_GOODS_NUM];
    double goods_cost[MAX_GOODS_NUM];
    bool  buyuav= false;
    bool  GetEnemyInitFlag=true;
    int  EnemyParkingX;
    int  EnemyParkingY;
    int Attack_Flag = 0;
    int Defend_Flag = 0;
    int attack_no[4];
    vector<int> DONENO;
    int Enemy_existtag[MAX_UAV_NUM];
    int Enemy_attacktag[MAX_UAV_NUM];
    vector<Point2f>  VecRangeXY;
    bool INIT=true;
    int buy_defend_counts = 0;




    /** @fn     int RecvJuderData(OS_SOCKET hSocket, char *pBuffer)
     *  @brief	接受数据
     *	@param  -I   - OS_SOCKET hSocket
     *	@param  -I   - char * pBuffer
     *	@return int
     */
    int RecvJuderData(OS_SOCKET hSocket, char *pBuffer)
    {
        int         nRecvLen = 0;
        int         nLen = 0;

        while (1)
        {
            // 接受头部长度
            nLen = OSRecv(hSocket, pBuffer + nRecvLen, MAX_SOCKET_BUFFER);
            if (nLen <= 0)
            {
                printf("recv error\n");
                return nLen;
            }

            nRecvLen += nLen;

            if (nRecvLen >= SOCKET_HEAD_LEN)
            {
                break;
            }
        }

        int         nJsonLen = 0;
        char        szLen[10] = { 0 };

        memcpy(szLen, pBuffer, SOCKET_HEAD_LEN);

        nJsonLen = atoi(szLen);

        while (nRecvLen < (SOCKET_HEAD_LEN + nJsonLen))
        {
            // 说明数据还没接受完
            nLen = OSRecv(hSocket, pBuffer + nRecvLen, MAX_SOCKET_BUFFER);
            if (nLen <= 0)
            {
                printf("recv error\n");
                return nLen;
            }

            nRecvLen += nLen;
        }

        return 0;
    }

    /** @fn     int SendJuderData(OS_SOCKET hSocket, char *pBuffer, int nLen)
     *  @brief	发送数据
     *	@param  -I   - OS_SOCKET hSocket
     *	@param  -I   - char * pBuffer
     *	@param  -I   - int nLen
     *	@return int
     */
    int SendJuderData(OS_SOCKET hSocket, char *pBuffer, int nLen)
    {
        int     nSendLen = 0;
        int     nLenTmp = 0;

        while (nSendLen < nLen)
        {
            nLenTmp = OSSend(hSocket, pBuffer + nSendLen, nLen - nSendLen);
            if (nLenTmp < 0)
            {
                return -1;
            }

            nSendLen += nLenTmp;
        }

        return 0;
    }


    /** @fn     void AlgorithmCalculationFun()
     *  @brief	学生的算法计算， 参数什么的都自己写，
     *	@return void
     */

    //**********************代码*************************//




    struct Node
    {
        Node()
        {
            Parant=NULL;
            Child=NULL;
        }
        ~Node()
        {
            delete Parant;
            delete Child;
        }
        int				x;
        int				y;
        double			g;		//权值
        double			h;		//到终点的距离
        double			f;		//估价值

        Node	*		Parant;
        Node	*		Child;

    };

    std::vector<Node *>				m_OpenList;
    std::vector<Node *>::iterator	m_OpenListIter;
    std::vector<Node *>				m_CloseList;
    std::vector<Node *>::iterator	m_CloseListIter;
    std::vector<Node *>				m_ResultList;
    std::vector<Node *>::iterator	m_ResultListIter;



    //搜索Open中估价值最小的点
    Node * SearchOpenSmallestNode()
    {
        Node * p;
        m_OpenListIter=m_OpenList.begin();
        p=*m_OpenListIter;
        for (;m_OpenListIter!=m_OpenList.end();m_OpenListIter++)
        {
            if((*m_OpenListIter)->f<p->f)
            {
                p=*m_OpenListIter;
            }
        }
        return p;
    }


    bool FindInOpen(Node * param)
    {
        m_OpenListIter=m_OpenList.begin();
        for (;m_OpenListIter!=m_OpenList.end();m_OpenListIter++)
        {
            if ((*m_OpenListIter)->x==param->x&&(*m_OpenListIter)->y==param->y)
            {
                return true;
            }
        }
        return false;
    }

    bool FindInClose(Node * param)
    {
        m_CloseListIter=m_CloseList.begin();
        for (;m_CloseListIter!=m_CloseList.end();m_CloseListIter++)
        {
            if ((*m_CloseListIter)->x==param->x&&(*m_CloseListIter)->y==param->y)
            {
                return true;
            }
        }
        return false;
    }

    void UpdateValue(Node * Param)
    {

    }

    void ChildNode(Node * parnode,int x,int y,int xend,int yend )
    {


        Node * NewNode=new Node;
        NewNode->x=x;
        NewNode->y=y;


        bool inOpentip=FindInOpen(NewNode);
        bool inClosetip=FindInClose(NewNode);

        //不在OPEN表和CLOSE表中
        if (!inOpentip&&!inClosetip)
        {
            //求估价值
            NewNode->g=parnode->g+1;
            NewNode->h=(double)sqrtf((NewNode->x-xend)*(NewNode->x-xend)+(NewNode->y-yend)*(NewNode->y-yend));
            NewNode->f=NewNode->g+NewNode->h;
            NewNode->Parant=parnode;
            parnode->Child=NewNode;

            //加入Open链表
            m_OpenList.push_back(NewNode);
        }
        else if (inOpentip)
        {
            //求估价值
            double g=parnode->g+1;
            double h=(double)sqrtf((NewNode->x-xend)*(NewNode->x-xend)+(NewNode->y-yend)*(NewNode->y-yend));
            double f=g+h;

            bool Updatetip=false;
            m_OpenListIter=m_OpenList.begin();
            for (;m_OpenListIter!=m_OpenList.end();m_OpenListIter++)
            {
                if ((*m_OpenListIter)->x==NewNode->x&&(*m_OpenListIter)->y==NewNode->y)
                {
                    if (f<(*m_OpenListIter)->f)
                    {
                        Updatetip=true;
                    }
                }
            }

            if (Updatetip)
            {
                //更新该节点的估价值
                NewNode->g=g;
                NewNode->h=h;
                NewNode->f=f;
                NewNode->Parant=parnode;
                parnode->Child=NewNode;

                //更新该节点的子节点估价值	此函数嫌麻烦，所以没有加（增加此函数 我觉得对提升搜索效率没多大帮助，不加也能得到最优解）
                //由于我没有写此函数，所以Node结构体里的Child随便定义的（如果各位想增加此函数，只需根据需求，确定四方向寻找还是八方向寻找，定义Child为数组就行 4或8 例：Node * Child[4];） 因为我没有用到此Child 所以定义为Node * Child;
                //此函数只需一个递归，把所有子节点的子节点的……的g值-1 然后重新计算f=g+h就行了 一直递归到无Child为止
                //UpdateValue(NewNode);
            }
        }
        else if (inClosetip)
        {
            //求估价值
            double g=parnode->g+1;
            double h=(double)sqrtf((NewNode->x-xend)*(NewNode->x-xend)+(NewNode->y-yend)*(NewNode->y-yend));
            double f=g+h;

            bool Updatetip=false;
            m_CloseListIter=m_CloseList.begin();
            for (;m_CloseListIter!=m_CloseList.end();m_CloseListIter++)
            {
                if ((*m_CloseListIter)->x==NewNode->x&&(*m_CloseListIter)->y==NewNode->y)
                {
                    if (f<(*m_CloseListIter)->f)
                    {
                        Updatetip=true;
                        m_CloseList.erase(m_CloseListIter);	//删除Close
                        break;
                    }
                }
            }

            if (Updatetip)
            {
                //更新该节点的估价值
                NewNode->g=g;
                NewNode->h=h;
                NewNode->f=f;
                NewNode->Parant=parnode;
                parnode->Child=NewNode;

                m_OpenList.push_back(NewNode);

                //更新该节点的所有子节点估价值	此函数嫌麻烦，所以没有加（增加此函数 我觉得对提升搜索效率没多大帮助，不加也能得到最优解）
                //由于我没有写此函数，所以Node结构体里的Child随便定义的（如果各位想增加此函数，只需根据需求，确定四方向寻找还是八方向寻找，定义Child为数组就行 4或8 例：Node * Child[4];） 因为我没有用到此Child 所以定义为Node * Child;
                //此函数只需一个递归，把所有子节点的子节点的……的g值-1 然后重新计算f=g+h就行了 一直递归到无Child为止
                //UpdateValue(NewNode);
            }
        }

    }

    void GetResult(Node * Res)
    {
        if (Res!=NULL)
        {
            m_ResultList.push_back(Res);
        }
        if (Res->Parant!=NULL)
        {
            GetResult(Res->Parant);
        }
    }









    vector<Point2f> AstarAlgorithmFun(MAP_INFO *stmap,int z,int beginX,int beginY, int targetX, int targrtY)
    {
        /*
         * 生成路径算法
         * 生成地图加入障碍物和雾区
         * 生成初始代
         * 计算适应性分数
         * 杂交
         * 变异
         */
    //***********************获取一个z高度的二维平面地图******************//
        const int MapX =stmap->nMapX ;
        const int MapY =stmap->nMapY ;
        const int MapZ =z ;  //在高度为z层二维平面进行遗传算法
        int Map[MapX][MapY];
        memset (Map, 0x00, sizeof (Map));//初始化为0
    #pragma omp parallel for
        for (int i = 0; i < stmap->nBuildingNum; ++i) {

            if(stmap->astBuilding[i].nH>=z) //如果障碍物高度大于等于我想要取的平面那么这个障碍物有效
            {
                for (int j = 0; j <stmap->astBuilding[i].nL ; ++j) {
                    for (int k = 0; k <stmap->astBuilding[i].nW ; ++k) {
                        Map[j+stmap->astBuilding[i].nX][k+stmap->astBuilding[i].nY]=1;
                    }
                }

            }

        }



    //    for (int i = 0; i < sizeof (stmap->astFog) / sizeof (stmap->astFog[0]); ++i) {
    //
    //        if(stmap->astFog[i].nT>=z) //如果雾区高度大于等于我想要取的平面那么这个大雾有效
    //        {
    //            for (int j = 0; j <stmap->astFog[i].nL ; ++j) {
    //                for (int k = 0; k <stmap->astFog[i].nW ; ++k) {
    //                    Map[j+stmap->astFog[i].nX-1][k+stmap->astFog[j].nY-1]=2;
    //                }
    //            }
    //
    //        }
    //
    //    }
        //***********************获取一个z高度的二维平面地图******************//
        vector<Point2f> vecvecp2_Memory ; //用于记录可到达终点的路径
        m_ResultList.clear ();
        m_OpenList.clear ();
        m_CloseList.clear ();




        int startx=beginX,starty=beginY,endx=targetX,endy=targrtY;

        Node	* m_StartPos=new Node;

        m_StartPos->x=startx;
        m_StartPos->y=starty;
        m_StartPos->g=0;
        m_StartPos->h=(double)sqrtf((m_StartPos->x-endx)*(m_StartPos->x-endx)+(m_StartPos->y-endy)*(m_StartPos->y-endy));
        m_StartPos->f=m_StartPos->g+m_StartPos->h;

        m_OpenList.push_back(m_StartPos);


        while (m_OpenList.size())
        {
            //搜索Open中估价值最小的点
            Node * Smallest=SearchOpenSmallestNode();
            //char zcjBuff[256]={0};
            //sprintf(zcjBuff,"搜索出的最小估价值--------------\n");
            //OutputDebugStringA(zcjBuff);
            //sprintf(zcjBuff,"x:%d_y:%d_ value:%f\n",Smallest->x,Smallest->y,Smallest->f);
            //OutputDebugStringA(zcjBuff);
            //并从Open表中删除该估价值最小的点
            m_OpenListIter=m_OpenList.begin();
            while(m_OpenListIter!=m_OpenList.end())
            {
                if((*m_OpenListIter)->x==Smallest->x&&(*m_OpenListIter)->y==Smallest->y)
                {
                    m_OpenListIter=m_OpenList.erase(m_OpenListIter);
                    break;
                }
                else
                {
                    m_OpenListIter++;
                }
            }

            //cout << Smallest->x <<"!!!!" <<Smallest->y << endl;
            //目标点
            if(Smallest->x==endx&&Smallest->y==endy)
            {
                //sprintf(zcjBuff,"找到目标点输出路径---------------------\n");
                // OutputDebugStringA(zcjBuff);

                GetResult(Smallest);



                Point2f close_route;
                reverse (m_ResultList.begin (),m_ResultList.end ());
                for (auto p : m_ResultList) {
                    close_route.x=p->x;
                    close_route.y=p->y;
                    //cout << "*********" << endl;
                    //cout << "(" << p->x << "," << p->y << ")" << endl;
                    vecvecp2_Memory.push_back (close_route);

                }

                m_ResultListIter=m_ResultList.begin();
                for (;m_ResultListIter!=m_ResultList.end();m_ResultListIter++)
                {
                    Node *Resultpos=*m_ResultListIter;
                    // sprintf(zcjBuff,"x:%d_y:%d_ value:%f\n",Resultpos->x,Resultpos->y,Resultpos->f);
                    //OutputDebugStringA(zcjBuff);
                }

                //getchar();
                //cout<<"////////////------------//////////"<<vecvecp2_Memory.size ()<<endl;
                return vecvecp2_Memory ;
            }

            //检索Smallest的子节点	//如果是四方向搜索  可以把东南、西南、西北、东北注释掉 一般四方向搜索就够了 八方向搜索没必要 可以作为参考

            if (!(Smallest->x+1>=MapX||Map[Smallest->x+1][Smallest->y]!=0))	//超出边界
            {
                ChildNode(Smallest,Smallest->x+1,Smallest->y,endx,endy);//东
            }
            if (!(Smallest->y-1<0||Smallest->x+1>=MapX||Map[Smallest->x+1][Smallest->y-1]!=0))	//超出边界
            {
                ChildNode(Smallest,Smallest->x+1,Smallest->y-1,endx,endy);//东南
            }
            if (!(Smallest->y-1<0||Map[Smallest->x][Smallest->y-1]!=0))	//超出边界
            {
                ChildNode(Smallest,Smallest->x,Smallest->y-1,endx,endy);//南
            }
            if (!(Smallest->x-1<0||Smallest->y-1<0||Map[Smallest->x-1][Smallest->y-1]!=0))	//超出边界
            {
                ChildNode(Smallest,Smallest->x-1,Smallest->y-1,endx,endy);//西南
            }
            if (!(Smallest->x-1<0||Map[Smallest->x-1][Smallest->y]!=0))	//超出边界
            {
                ChildNode(Smallest,Smallest->x-1,Smallest->y,endx,endy);//西
            }
            if (!(Smallest->x-1<0||Smallest->y+1>=MapY||Map[Smallest->x-1][Smallest->y+1]!=0))	//超出边界
            {
                ChildNode(Smallest,Smallest->x-1,Smallest->y+1,endx,endy);//西北
            }
            if (!(Smallest->y+1>=MapY||Map[Smallest->x][Smallest->y+1]!=0))	//超出边界
            {
                ChildNode(Smallest,Smallest->x,Smallest->y+1,endx,endy);//北
            }
            if (!(Smallest->x+1>=MapX||Smallest->y+1>=MapY||Map[Smallest->x+1][Smallest->y+1]!=0))	//超出边界
            {
                ChildNode(Smallest,Smallest->x+1,Smallest->y+1,endx,endy);//东北
            }







            {
                //sprintf(zcjBuff,"Open链表信息---------------------\n");
                //OutputDebugStringA(zcjBuff);
                m_OpenListIter=m_OpenList.begin();
                for (;m_OpenListIter!=m_OpenList.end();m_OpenListIter++)
                {
                    Node *Openpos=*m_OpenListIter;
                    //sprintf(zcjBuff,"x:%d_y:%d_ value:%f\n",Openpos->x,Openpos->y,Openpos->f);
                    //OutputDebugStringA(zcjBuff);
                }
            }



            //估值最小的点加到Close里
            m_CloseList.push_back(Smallest);



            {
                //sprintf(zcjBuff,"Close链表信息---------------------\n");
                // OutputDebugStringA(zcjBuff);
                m_CloseListIter=m_CloseList.begin();
                for (;m_CloseListIter!=m_CloseList.end();m_CloseListIter++)
                {

                    Node *Closepos=*m_CloseListIter;
                    // sprintf(zcjBuff,"x:%d_y:%d_ value:%f\n",Closepos->x,Closepos->y,Closepos->f);
                    // OutputDebugStringA(zcjBuff);
                }
            }

            //OutputDebugStringA("++++++++++++++++++++++++++++++++++++++++");

        }

        //getchar();




    }
    //规划到路径点上方
    vector<Point3f> point_path(MAP_INFO *opmap, UAV *target_plane, int x,int y)
    {
        int planeX = target_plane->nX;
        int planeY = target_plane->nY;
        int targetX = x;
        int targetY = y;

        // int go_ChromoLength=sqrt(pow((goodsX-planeX),2)+pow((goodsY-planeY),2))*15;
        // int back_ChromoLength=sqrt(pow((goodsX-targetX),2)+pow((goodsY-targetY),2))*15;
        vector<Point3f> total_path;
        vector<Point2f> go;
        int best_go = 9999;
        int unload_height = 0;
        set<int> change_height;
        for (int i = 0; i < opmap->nBuildingNum; i++) {
            change_height.insert(opmap->astBuilding[i].nH + 1);
        }
        change_height.insert(opmap->nHLow);
        for (set<int>::iterator iter = change_height.begin(); iter != change_height.end(); iter++) {

            if (*iter >= opmap->nHLow) {
                int nfly_height = min(*iter, opmap->nHHigh);
                vector<Point2f> v1 = AstarAlgorithmFun (opmap, nfly_height, planeX, planeY, targetX, targetY);
                int tmp_time = v1.size();
                if ((tmp_time + (nfly_height - unload_height) * 2) < best_go) {
                    best_go = tmp_time;
                    unload_height = nfly_height;
                    go.assign(v1.begin(), v1.end());

                } else
                    break;
            }
        }
        //简化路径，不走无效步
        for(int m = 0; m < go.size(); m ++){
            Point2f tmp_point = go[m];
            for(int n = m + 1; n < go.size(); n++){
                if(go[n].x == tmp_point.x && go[n].y == tmp_point.y){
                    go.erase(go.begin() + m, go.begin() + n);
                    n = m + 1;   //go长度改变，所以n要回到m下一位继续寻找
                }
            }
        }
        Point3f value;
        if(!go.empty ()) {
            if (target_plane->nZ < unload_height) {
                for (int i = target_plane->nZ + 1; i < unload_height; i++) {
                    value.x = planeX;
                    value.y = planeY;
                    value.z = i;
                    total_path.push_back (value);
                }
            } else if (target_plane->nZ > unload_height) {
                for (int i = target_plane->nZ - 1; i < unload_height; i--) {
                    value.x = planeX;
                    value.y = planeY;
                    value.z = i;
                    total_path.push_back (value);
                }
            }
            for (int i = 0; i < go.size (); i++) {
                value.x = go[i].x;
                value.y = go[i].y;
                value.z = unload_height;
                total_path.push_back (value);
            }
            for (int i = unload_height - 1; i >=opmap->nHLow-2; i--) {
                value.x = targetX;
                value.y = targetY;
                value.z = i;
                total_path.push_back (value);
            }
        }


    //    for (auto p:total_path)
    //    {
    //        cout<<p.x<<":"<<p.y<<":"<<p.z<<endl;
    //    }
        return total_path;
    }

    //为每个路径规划一个最好平面
    vector<Point3f> optimal_path(MAP_INFO *opmap, UAV *target_plane, GOODS target_goods)
    {
        int planeX = target_plane->nX;
        int planeY = target_plane->nY;
        int goodsX = target_goods.nStartX;
        int goodsY = target_goods.nStartY;
        int targetX = target_goods.nEndX;
        int targetY = target_goods.nEndY;

        // int go_ChromoLength=sqrt(pow((goodsX-planeX),2)+pow((goodsY-planeY),2))*15;
        // int back_ChromoLength=sqrt(pow((goodsX-targetX),2)+pow((goodsY-targetY),2))*15;
        vector<Point3f> total_path;
        vector<Point2f> go;
        int best_go = 9999;
        int unload_height = 0;
        set<int> change_height;
        for (int i = 0; i < opmap->nBuildingNum; i++) {
            change_height.insert(opmap->astBuilding[i].nH + 1);
        }
        change_height.insert(opmap->nHLow);
        for (set<int>::iterator iter = change_height.begin(); iter != change_height.end(); iter++) {

            if (*iter >= opmap->nHLow) {
                int nfly_height = min(*iter, opmap->nHHigh);
                vector<Point2f> v1 = AstarAlgorithmFun (opmap, nfly_height, planeX, planeY, goodsX, goodsY);
                int tmp_time = v1.size();
                if ((tmp_time + (nfly_height - unload_height) * 2) < best_go) {
                    best_go = tmp_time;
                    unload_height = nfly_height;
                    go.assign(v1.begin(), v1.end());

                } else
                    break;
            }
        }
        //简化路径，不走无效步
        for(int m = 0; m < go.size(); m ++){
            Point2f tmp_point = go[m];
            for(int n = m + 1; n < go.size(); n++){
                if(go[n].x == tmp_point.x && go[n].y == tmp_point.y){
                    go.erase(go.begin() + m, go.begin() + n);
                    n = m + 1;   //go长度改变，所以n要回到m下一位继续寻找
                }
            }
        }
        target_plane->fly_height = unload_height;
        vector<Point2f> back;
        int best_back = 9999;
        int loading_height = 0;
        for (int i = 0; i < opmap->nBuildingNum; i++) {
            change_height.insert(opmap->astBuilding[i].nH + 1);
        }
        change_height.insert(opmap->nHLow);
        for (set<int>::iterator iter = change_height.begin(); iter != change_height.end(); iter++) {
            if (*iter >= opmap->nHLow) {
                int nfly_height = min(*iter, opmap->nHHigh);
                vector<Point2f> v2 = AstarAlgorithmFun (opmap, nfly_height, goodsX, goodsY, targetX, targetY);
                int tmp_time = v2.size();
                if ((tmp_time + (nfly_height - loading_height) * 2) < best_back) {
                    best_back = tmp_time;
                    loading_height = nfly_height;
                    back.assign(v2.begin(), v2.end());
                } else
                    break;
            }
        }
        //简化路径，不走无效步
        for(int m = 0; m < back.size(); m ++){
            Point2f tmp_point = back[m];
            for(int n = m + 1; n < back.size(); n++){
                if(back[n].x == tmp_point.x && back[n].y == tmp_point.y){
                    back.erase(back.begin() + m, back.begin() + n);
                    n = m + 1;   //back长度改变，所以n要回到m下一位继续寻找
                }
            }
        }
        target_plane->load_height = loading_height;
        Point3f value;
        if(!go.empty () && !back.empty ()) {
            if (target_plane->nZ < unload_height) {
                for (int i = target_plane->nZ + 1; i < unload_height; i++) {
                    value.x = planeX;
                    value.y = planeY;
                    value.z = i;
                    total_path.push_back (value);
                }
            } else if (target_plane->nZ > unload_height) {
                for (int i = target_plane->nZ - 1; i < unload_height; i--) {
                    value.x = planeX;
                    value.y = planeY;
                    value.z = i;
                    total_path.push_back (value);
                }
            }
            for (int i = 0; i < go.size (); i++) {
                value.x = go[i].x;
                value.y = go[i].y;
                value.z = unload_height;
                total_path.push_back (value);
            }
            for (int i = unload_height - 1; i >= 0; i--) {
                value.x = goodsX;
                value.y = goodsY;
                value.z = i;
                total_path.push_back (value);
            }
            target_plane->go_stepNum = total_path.size ();
            for (int i = 1; i < loading_height; i++) {
                value.x = goodsX;
                value.y = goodsY;
                value.z = i;
                total_path.push_back (value);
            }
            for (int i = 0; i < back.size (); i++) {
                value.x = back[i].x;
                value.y = back[i].y;
                value.z = loading_height;
                total_path.push_back (value);
            }
            for (int i = loading_height - 1; i >= 0; i--) {
                value.x = targetX;
                value.y = targetY;
                value.z = i;
                total_path.push_back (value);
            }
            for(int i = 0; i < opmap->nHLow; i++){
                value.x = targetX;
                value.y = targetY;
                value.z = i;
                total_path.push_back(value);
            }
        }


    //    for (auto p:total_path)
    //    {
    //        cout<<p.x<<":"<<p.y<<":"<<p.z<<endl;
    //    }
        return total_path;
    }

    double CostFun(GOODS target_goods,int x, int y,int z)
    {
        double cost;
        double distance=sqrt(pow((target_goods.nStartX-x),2)*((target_goods.nStartY-y),2))+
                        sqrt(pow((target_goods.nStartX-target_goods.nEndX),2)*((target_goods.nStartY-target_goods.nEndY),2));
        if(distance+z<target_goods.nLeftTime) {
            cost = (target_goods.nValue * target_goods.nValue) / distance;
            return cost;
        }
        else
            return 0;
    }

    void  AlgorithmCalculationFun(MAP_INFO *pstMap, MATCH_STATUS * pstMatch, FLAY_PLANE *pstFlayPlane) {


    //    for (int n1 = 0; n1 <pstMatch->nUavWeNum ; ++n1) {
    //        cout<<n1<<"attack_flag?????????????"<<pstMatch->astWeUav[n1].attack_flag<<endl;
    //        cout << "UAV" << n1 << endl;
    //        for(int m = 0; m < pstMatch->astWeUav[n1].route.size (); m ++){
    //            cout << "(" << pstMatch->astWeUav[n1].route[m].x << ", " << pstMatch->astWeUav[n1].route[m].y << ", " << pstMatch->astWeUav[n1].route[m].z << ")" << endl;
    //        }
    //    }

        //5-28  停机坪防守，若有对方飞机在已方停机坪上方。 买一个最便宜的飞机去撞掉
        bool secure_flag = false;
        for (int i = 0; i < pstMatch->nUavEnemyNum; ++i) {
            if(pstMatch->astEnemyUav[i].nX == pstMap->nParkingX && pstMatch->astEnemyUav[i].nY == pstMap->nParkingY ){
                secure_flag = true;
                for (int k = 0; k < pstMatch->nUavWeNum; ++k) {            //当我方停机坪上方
                    if(pstMatch->astWeUav[k].nX == pstMap->nParkingX && pstMatch->astWeUav[k].nY == pstMap->nParkingY && pstMatch->astWeUav[k].nZ > 0)
                        secure_flag = false;
                }
            }
        }



        //寻找最便宜飞机
        int cheapest_no=0;
        int cheapest_value=pstMap->astUavPrice[0].nValue;
        for (int l1 = 1; l1 <pstMap->nUavPriceNum ; ++l1) {

            if(pstMap->astUavPrice[l1].nValue<cheapest_value)
            {
                cheapest_value=pstMap->astUavPrice[l1].nValue;
                cheapest_no=l1;
            }

        }

        //更新敌方飞机存在
        memset(Enemy_existtag,0,sizeof(Enemy_existtag));
        for(int i=0;i<pstMatch->nUavEnemyNum;i++)
        {
            if(pstMatch->astEnemyUav[i].nStatus!=1)
                Enemy_existtag[pstMatch->astEnemyUav[i].nNO]=1;
        }

        //cout<<cheapest_no<<":::"<<pstMap->astUavPrice[cheapest_no].nValue<<endl;



        //清空输出状态
    //    FLAY_PLANE *emptyplane;
    //    pstFlayPlane=emptyplane;
    //    for (int m1 = 0; m1 <pstFlayPlane->nUavNum ; ++m1) {
    //        cout << "NO " << pstFlayPlane->astUav[m1].nNO << " x: "
    //             << pstFlayPlane->astUav[m1].nX << " y: " << pstFlayPlane->astUav[m1].nY
    //             <<" z: " << pstFlayPlane->astUav[m1].nZ << " nstatus " << pstFlayPlane->astUav[m1].nStatus
    //             << " goodsno " <<pstFlayPlane->astUav[m1].nGoodsNo<< endl;
    //    }
        //检测飞机状态
        for(int i=0;i<pstMatch->nUavWeNum;i++)
        {
            if(pstMatch->astWeUav[i].nStatus==1)
            {
                pstMatch->astWeUav[i].route.clear ();
                pstMatch->astWeUav[i].nX=-1;
                pstMatch->astWeUav[i].nY=-1;
                pstMatch->astWeUav[i].nZ=-1;
            }
        }
    //获取对方停机坪
        if (GetEnemyInitFlag)
        {
            for (int i = 0; i <pstMatch->nUavEnemyNum ; ++i) {
                if (pstMatch->astEnemyUav[i].nZ==1)
                {
                    EnemyParkingX=pstMatch->astEnemyUav[i].nX;
                    EnemyParkingY=pstMatch->astEnemyUav[i].nY;
                    GetEnemyInitFlag= false;
                }
            }
        }


  if(INIT)
  {

        int X1=pstMap->nMapX/4;
        int X2=pstMap->nMapX/4*3;
        int Y1=pstMap->nMapY/4;
        int Y2=pstMap->nMapY/4*3;
        Point2f X1Y2; //1 左上
        Point2f X2Y2; //2 右上
        Point2f X2Y1; //3 右下
        Point2f X1Y1; //4 左下
        X1Y2.x=X1;
        X1Y2.y=Y2;
        X2Y2.x=X2;
        X2Y2.y=Y2;
        X2Y1.x=X2;
        X2Y1.y=Y1;
        X1Y1.x=X1;
        X1Y1.y=Y1;
        for (int i1 = 0; i1 < pstMap->nBuildingNum; ++i1) {
            if(pstMap->astBuilding[i1].nH > pstMap->nHLow){
                if(X1Y1.x >= pstMap->astBuilding[i1].nX && X1Y1.x <= pstMap->astBuilding[i1].nX + pstMap->astBuilding[i1].nL
                   && X1Y1.y >= pstMap->astBuilding[i1].nY && X1Y1.y <= pstMap->astBuilding[i1].nY + pstMap->astBuilding[i1].nW){
                    int leftdistance=abs (X1-pstMap->astBuilding[i1].nX);
                    int rightdistance=abs (X1-(pstMap->astBuilding[i1].nX+pstMap->astBuilding[i1].nL));
                    int botdistance=abs (Y1-pstMap->astBuilding[i1].nY);
                    int topdistance=abs (Y1-(pstMap->astBuilding[i1].nY+pstMap->astBuilding[i1].nW));
                    int min=leftdistance;
                    {
                        X1Y1.x=pstMap->astBuilding[i1].nX-1;
                    }
                    if(min>rightdistance)
                    {
                        min=rightdistance;
                        X1Y1.x=(pstMap->astBuilding[i1].nX+pstMap->astBuilding[i1].nL+1);

                    }
                    if(min>botdistance)
                    {
                        min=botdistance;
                        X1Y1.y=pstMap->astBuilding[i1].nY-1;
                    }
                    if(min>topdistance)
                    {

                        min=topdistance;
                        X1Y1.y=(pstMap->astBuilding[i1].nY+pstMap->astBuilding[i1].nW+1);
                    }


                }
                if(X2Y1.x >= pstMap->astBuilding[i1].nX && X2Y1.x <= pstMap->astBuilding[i1].nX + pstMap->astBuilding[i1].nL
                   && X2Y1.y >= pstMap->astBuilding[i1].nY && X2Y1.y <= pstMap->astBuilding[i1].nY + pstMap->astBuilding[i1].nW){

                    int leftdistance=abs (X2-pstMap->astBuilding[i1].nX);
                    int rightdistance=abs (X2-(pstMap->astBuilding[i1].nX+pstMap->astBuilding[i1].nL));
                    int botdistance=abs (Y1-pstMap->astBuilding[i1].nY);
                    int topdistance=abs (Y1-(pstMap->astBuilding[i1].nY+pstMap->astBuilding[i1].nW));
                    int min=leftdistance;
                    {
                        X2Y1.x=pstMap->astBuilding[i1].nX-1;
                    }
                    if(min>rightdistance)
                    {
                        min=rightdistance;
                        X2Y1.x=(pstMap->astBuilding[i1].nX+pstMap->astBuilding[i1].nL+1);

                    }
                    if(min>botdistance)
                    {
                        min=botdistance;
                        X2Y1.y=pstMap->astBuilding[i1].nY-1;
                    }
                    if(min>topdistance)
                    {

                        //min=topdistance;
                        X2Y1.y=(pstMap->astBuilding[i1].nY+pstMap->astBuilding[i1].nW+1);
                    }

                }
                if(X1Y2.x >= pstMap->astBuilding[i1].nX && X1Y2.x <= pstMap->astBuilding[i1].nX + pstMap->astBuilding[i1].nL
                   && X1Y2.y >= pstMap->astBuilding[i1].nY && X1Y2.y <= pstMap->astBuilding[i1].nY + pstMap->astBuilding[i1].nW){

                    int leftdistance=abs (X1-pstMap->astBuilding[i1].nX);
                    int rightdistance=abs (X1-(pstMap->astBuilding[i1].nX+pstMap->astBuilding[i1].nL));
                    int botdistance=abs (Y2-pstMap->astBuilding[i1].nY);
                    int topdistance=abs (Y2-(pstMap->astBuilding[i1].nY+pstMap->astBuilding[i1].nW));
                    int min=leftdistance;
                    {
                        X1Y2.x=pstMap->astBuilding[i1].nX-1;
                    }
                    if(min>rightdistance)
                    {
                        min=rightdistance;
                        X1Y2.x=(pstMap->astBuilding[i1].nX+pstMap->astBuilding[i1].nL+1);

                    }
                    if(min>botdistance)
                    {
                        min=botdistance;
                        X1Y2.y=pstMap->astBuilding[i1].nY-1;
                    }
                    if(min>topdistance)
                    {

                        //min=topdistance;
                        X1Y2.y=(pstMap->astBuilding[i1].nY+pstMap->astBuilding[i1].nW+1);
                    }

                }
                if(X2Y2.x >= pstMap->astBuilding[i1].nX && X2Y2.x <= pstMap->astBuilding[i1].nX + pstMap->astBuilding[i1].nL
                   && X2Y2.y >= pstMap->astBuilding[i1].nY && X2Y2.y <= pstMap->astBuilding[i1].nY + pstMap->astBuilding[i1].nW){

                    int leftdistance=abs (X2-pstMap->astBuilding[i1].nX);
                    int rightdistance=abs (X2-(pstMap->astBuilding[i1].nX+pstMap->astBuilding[i1].nL));
                    int botdistance=abs (Y2-pstMap->astBuilding[i1].nY);
                    int topdistance=abs (Y2-(pstMap->astBuilding[i1].nY+pstMap->astBuilding[i1].nW));
                    int min=leftdistance;
                    {
                        X2Y2.x=pstMap->astBuilding[i1].nX-1;
                    }
                    if(min>rightdistance)
                    {
                        min=rightdistance;
                        X2Y2.x=(pstMap->astBuilding[i1].nX+pstMap->astBuilding[i1].nL+1);

                    }
                    if(min>botdistance)
                    {
                        min=botdistance;
                        X2Y2.y=pstMap->astBuilding[i1].nY-1;
                    }
                    if(min>topdistance)
                    {

                        //min=topdistance;
                        X2Y2.y=(pstMap->astBuilding[i1].nY+pstMap->astBuilding[i1].nW+1);
                    }

                }
            }
        }


        VecRangeXY.push_back(X1Y2);
        VecRangeXY.push_back(X2Y2);
        VecRangeXY.push_back(X2Y1);
        VecRangeXY.push_back(X1Y1);
      INIT=false;
  }




        GOODS goodsno[MAX_GOODS_NUM];
        for (int n = 0; n <pstMatch->nGoodsNum ; ++n) {
            goodsno[pstMatch->astGoods[n].nNO]=pstMatch->astGoods[n];
        }



        /*
         * 生成路径算法
         * 生成地图加入障碍物和雾区
         * 生成初始代
         * 计算适应性分数
         * 杂交
         * 变异
         */
    //    for(int i=0;i<pstMatch->nGoodsNum;i++)
    //    {
    //        total_goods[pstMatch->astGoods[i].nNO].nNO=pstMatch->astGoods[i].nNO;
    //        total_goods[pstMatch->astGoods[i].nNO].nEndX=pstMatch->astGoods[i].nEndX;
    //        total_goods[pstMatch->astGoods[i].nNO].nEndY=pstMatch->astGoods[i].nEndY;
    //        total_goods[pstMatch->astGoods[i].nNO].nLeftTime=pstMatch->astGoods[i].nLeftTime;
    //        total_goods[pstMatch->astGoods[i].nNO].nStartX=pstMatch->astGoods[i].nStartX;
    //        total_goods[pstMatch->astGoods[i].nNO].nStartY=pstMatch->astGoods[i].nStartY;
    //        total_goods[pstMatch->astGoods[i].nNO].nState=pstMatch->astGoods[i].nState;
    //        total_goods[pstMatch->astGoods[i].nNO].nWeight=pstMatch->astGoods[i].nWeight;
    //
    //    }

        for (int l = 0; l <pstMatch->nGoodsNum; ++l) {
            goods_cost[pstMatch->astGoods[l].nNO]=0;
        }

        srand((unsigned)time(NULL));
        //vector <vector<double >> vecveccost;
        vector <double > veccost;

        static vector<int> UAV_NO;
        int enemy_no1 = pstMatch->astEnemyUav[0].nNO, enemy_no2 = pstMatch->astEnemyUav[1].nNO;
        int enemy_weight1 = pstMatch->astEnemyUav[0].nLoadWeight;
        int enemy_weight2 = pstMatch->astEnemyUav[1].nLoadWeight;

        //对刚开始的飞机进行重量排序, UAV_NO依次存放载重量逐渐减少的UAV的index，让载重最大的飞机先起飞
        if(pstMap->init_flag == 0){
            int tmp_weight[pstMatch->nUavWeNum];
            for(int i = 0; i < pstMatch->nUavWeNum; i ++){                     //创建一个临时载重数组
                tmp_weight[i] = pstMatch->astWeUav[i].nLoadWeight;
            }

            for(int i = 0; i < pstMatch->nUavWeNum; i++){
                int max_loadweight = tmp_weight[0];
                int max_loadweight_index = 0;
                for(int j = 1; j < pstMatch->nUavWeNum; j++){
                    if(tmp_weight[j] > max_loadweight){
                        max_loadweight = tmp_weight[j];
                        max_loadweight_index = j;
                    }
                }
                UAV_NO.push_back(max_loadweight_index);
                tmp_weight[max_loadweight_index] = 0;
            }

            //寻找地方载重前二UAV
            if(pstMatch->astEnemyUav[0].nLoadWeight < pstMatch->astEnemyUav[1].nLoadWeight){
                enemy_no1 = pstMatch->astEnemyUav[1].nNO;
                enemy_no2 = pstMatch->astEnemyUav[0].nNO;
                enemy_weight1 = pstMatch->astEnemyUav[1].nLoadWeight;
                enemy_weight2 = pstMatch->astEnemyUav[0].nLoadWeight;
            }
            for (int m = 2; m < pstMatch->nUavEnemyNum; ++m) {
                if(pstMatch->astEnemyUav[m].nLoadWeight > enemy_weight1){
                    enemy_no2 = enemy_no1;
                    enemy_no1 = pstMatch->astEnemyUav[m].nNO;
                    enemy_weight2 = enemy_weight1;
                    enemy_weight1 = pstMatch->astEnemyUav[m].nLoadWeight;
                }
                if(pstMatch->astEnemyUav[m].nLoadWeight < enemy_weight1 && pstMatch->astEnemyUav[m].nLoadWeight > enemy_weight2){
                    enemy_no2 = pstMatch->astEnemyUav[m].nNO;
                    enemy_weight2 = pstMatch->astEnemyUav[m].nLoadWeight;
                }
            }

            //对价值低的UAV进行进攻标志赋值
            pstMatch->astWeUav[UAV_NO[UAV_NO.size () - 1]].init_attack_flag = 1;
            //pstMatch->astWeUav[UAV_NO[UAV_NO.size () - 1]].init_attack_target = enemy_no1 + 100;    //让攻击0号的也能有取物功能
            pstMatch->astWeUav[UAV_NO[UAV_NO.size () - 1]].attack_no = enemy_no1;
            pstMatch->astWeUav[UAV_NO[UAV_NO.size () - 2]].init_attack_flag = 2;
            //pstMatch->astWeUav[UAV_NO[UAV_NO.size () - 2]].init_attack_target = enemy_no2 + 100;
            pstMatch->astWeUav[UAV_NO[UAV_NO.size () - 2]].attack_no = enemy_no2;
            UAV_NO.insert (UAV_NO.begin () + 1, UAV_NO[UAV_NO.size () - 1]);
            UAV_NO.insert (UAV_NO.begin () + 2, UAV_NO[UAV_NO.size () - 2]);
            UAV_NO.erase (UAV_NO.end () - 1);
            UAV_NO.erase (UAV_NO.end () - 1);
    //        cout << "UUUUUUUUUUUUUUUUUUUUUUUUUUUUU" << endl;
    //        for (int k = 0; k < UAV_NO.size(); ++k) {
    //            cout << UAV_NO[k] << endl;
    //        }
        }





    //    //更新可攻击飞机列表
    //    memset(attack_no, -1, sizeof(attack_no));
    //    for (int i = 0; i < pstMatch->nUavWeNum; i++) {
    //        if (pstMatch->astWeUav[i].nX==VecRangeXY[0].x && pstMatch->astWeUav[i].nY==VecRangeXY[0].y&&pstMatch->astWeUav[i].nStatus == 0 && pstMatch->astWeUav[i].attack_flag == 1 &&
    //            pstMatch->astWeUav[i].route.empty()) {
    //            attack_no[0] = pstMatch->astWeUav[i].nNO;
    //        }
    //        if (pstMatch->astWeUav[i].nX==VecRangeXY[1].x && pstMatch->astWeUav[i].nY==VecRangeXY[1].y&&pstMatch->astWeUav[i].nStatus == 0 && pstMatch->astWeUav[i].attack_flag == 2 &&
    //            pstMatch->astWeUav[i].route.empty()) {
    //            attack_no[1] = pstMatch->astWeUav[i].nNO;
    //        }
    //        if (pstMatch->astWeUav[i].nX==VecRangeXY[2].x && pstMatch->astWeUav[i].nY==VecRangeXY[2].y&&pstMatch->astWeUav[i].nStatus == 0 && pstMatch->astWeUav[i].attack_flag == 3 &&
    //            pstMatch->astWeUav[i].route.empty()) {
    //            attack_no[2] = pstMatch->astWeUav[i].nNO;
    //        }
    //        if (pstMatch->astWeUav[i].nX==VecRangeXY[3].x && pstMatch->astWeUav[i].nY==VecRangeXY[3].y&&pstMatch->astWeUav[i].nStatus == 0 && pstMatch->astWeUav[i].attack_flag == 4 &&
    //            pstMatch->astWeUav[i].route.empty()) {
    //            attack_no[3] = pstMatch->astWeUav[i].nNO;
    //        }
    //    }


        if(Attack_Flag != 0){
            pstMatch->astWeUav[pstMatch->nUavWeNum - 1].attack_flag = Attack_Flag;
            Attack_Flag = 0;

        }

        if(Defend_Flag != 0){
            pstMatch->astWeUav[pstMatch->nUavWeNum - 1].defend_flag = Defend_Flag;
            Defend_Flag = 0;
        }



    //    //检测区域可撞飞机
    //    for(int i=0;i<pstMatch->nUavEnemyNum;i++)
    //    {
    //        if(pstMatch->astEnemyUav[i].nZ<pstMap->nHLow && pstMatch->astEnemyUav[i].nGoodsNo==-1)
    //        {
    //            for(int j=0;j<pstMatch->nGoodsNum;j++)
    //            {
    //                if(pstMatch->astEnemyUav[i].nX==pstMatch->astGoods[j].nStartX && pstMatch->astEnemyUav[i].nY==pstMatch->astGoods[j].nStartY)
    //                {
    //                    if((pstMatch->astGoods[j].nEndX<(pstMap->nMapX/2)) && (pstMatch->astGoods[j].nEndY<(pstMap->nMapY/2)) && attack_no[3] !=-1)
    //                    {
    //                        pstMatch->astWeUav[attack_no[3]].route=point_path (pstMap, &pstMatch->astWeUav[attack_no[3]], pstMatch->astGoods[j].nEndX,pstMatch->astGoods[j].nEndY);
    //                        pstMatch->astWeUav[attack_no[3]].attack_flag=0;
    //                        break;
    //                    }
    //                    else if((pstMatch->astGoods[j].nEndX>(pstMap->nMapX/2)) && (pstMatch->astGoods[j].nEndY<(pstMap->nMapY/2)) && attack_no[2] !=-1)
    //                    {
    //                        pstMatch->astWeUav[attack_no[2]].route=point_path (pstMap, &pstMatch->astWeUav[attack_no[2]], pstMatch->astGoods[j].nEndX,pstMatch->astGoods[j].nEndY);
    //                        pstMatch->astWeUav[attack_no[2]].attack_flag=0;
    //                        break;
    //                    }
    //                    else if((pstMatch->astGoods[j].nEndX>(pstMap->nMapX/2) ) && (pstMatch->astGoods[j].nEndY>(pstMap->nMapY/2)) && attack_no[1] !=-1)
    //                    {
    //                        pstMatch->astWeUav[attack_no[1]].route=point_path (pstMap, &pstMatch->astWeUav[attack_no[1]], pstMatch->astGoods[j].nEndX,pstMatch->astGoods[j].nEndY);
    //                        pstMatch->astWeUav[attack_no[1]].attack_flag=0;
    //                        break;
    //                    }
    //                    else if((pstMatch->astGoods[j].nEndX < (pstMap->nMapX/2)) && (pstMatch->astGoods[j].nEndY > (pstMap->nMapY/2) )&& attack_no[0] !=-1)
    //                    {
    //                        pstMatch->astWeUav[attack_no[0]].route=point_path (pstMap, &pstMatch->astWeUav[attack_no[0]], pstMatch->astGoods[j].nEndX,pstMatch->astGoods[j].nEndY);
    //                        pstMatch->astWeUav[attack_no[0]].attack_flag=0;
    //                        break;
    //                    }
    //                }
    //            }
    //
    //        }
    //    }

        int AttParknum=0;
        for(int tmp_i=0;tmp_i<pstMatch->nUavWeNum;tmp_i++) { //遍历自己所有飞机
            int i = UAV_NO[tmp_i];
            if (pstMatch->astWeUav[i].attack_flag==-1&&pstMatch->astWeUav[i].nStatus!=1)
            {
                AttParknum++;
            }
            if(pstMatch->astWeUav[i].attack_flag == -1
            && pstMatch->astWeUav[i].nX == EnemyParkingX && pstMatch->astWeUav[i].nY == EnemyParkingY
            && pstMatch->astWeUav[i].route.empty() && pstMatch->astWeUav[i].nZ > 1){
                bool tmp1_flag = true;
                for (int r = 0; r < pstMatch->nUavWeNum; ++r) {
                    if(pstMatch->astWeUav[r].nX == EnemyParkingX && pstMatch->astWeUav[r].nY == EnemyParkingY
                       && pstMatch->astWeUav[r].nZ == pstMatch->astWeUav[i].nZ - 1
                       && pstMatch->astWeUav[r].route.empty ())
                        tmp1_flag = false;
                }
                if(tmp1_flag){
                    Point3f tmp_point;
                    tmp_point.x = pstMatch->astWeUav[i].nX;
                    tmp_point.y = pstMatch->astWeUav[i].nY;
                    tmp_point.z = pstMatch->astWeUav[i].nZ - 1;
                    pstMatch->astWeUav[i].route.push_back(tmp_point);
                }
            }
            //5-27  前期低载攻击
            if(pstMatch->astWeUav[i].init_attack_flag != 0 && pstMap->init_flag == -1&&pstMatch->astWeUav[i].attack_flag==0
               && ((pstMatch->astWeUav[i].route.empty () && pstMatch->astWeUav[i].nX == pstMap->nParkingX && pstMatch->astWeUav[i].nY == pstMap->nParkingY)
                   || (pstMatch->astWeUav[i].nX != pstMap->nParkingX || pstMatch->astWeUav[i].nY != pstMap->nParkingY))){
                if(pstMatch->astWeUav[i].nZ <= pstMap->nHLow - 1 && pstMatch->astWeUav[i].nX == pstMap->nParkingX && pstMatch->astWeUav[i].nY == pstMap->nParkingY){
    //                cout << i <<"~~bei_qianqidizai_guihua1" <<endl;
                    Point3f tmp_point;
                    tmp_point.x = pstMatch->astWeUav[i].nX;
                    tmp_point.y = pstMatch->astWeUav[i].nY;
                    tmp_point.z = pstMatch->astWeUav[i].nZ + 1;
                    pstMatch->astWeUav[i].route.push_back (tmp_point);
                }
                for (int j = 0; j < pstMatch->nUavEnemyNum; ++j) {
                    if(pstMatch->astEnemyUav[j].nNO == pstMatch->astWeUav[i].attack_no){
                        if(pstMatch->astEnemyUav[j].nX >= 0 && pstMatch->astEnemyUav[j].nY >= 0){
                            pstMatch->astWeUav[i].attack_target.x = pstMatch->astEnemyUav[j].nX;
                            pstMatch->astWeUav[i].attack_target.y = pstMatch->astEnemyUav[j].nY;
                            pstMatch->astWeUav[i].attack_target.z = pstMatch->astEnemyUav[j].nZ;
                        }
                    }
                }
                if(pstMatch->astWeUav[i].nZ >= pstMap->nHLow)
                {
                    //cout << i <<"~~bei_qianqidizai_guihua2" <<endl;
                    vector<Point2f> temper_route = AstarAlgorithmFun(pstMap, pstMatch->astWeUav[i].nZ, pstMatch->astWeUav[i].nX, pstMatch->astWeUav[i].nY,
                                                                     pstMatch->astWeUav[i].attack_target.x, pstMatch->astWeUav[i].attack_target.y);
                    pstMatch->astWeUav[i].route.clear();
                    if(!temper_route.empty ()){
                        temper_route.erase(temper_route.begin());
                    }
                    for(auto p:temper_route){
                        Point3f temper_point;
                        temper_point.x = p.x;
                        temper_point.y = p.y;
                        temper_point.z = pstMatch->astWeUav[i].nZ;
                        pstMatch->astWeUav[i].route.push_back(temper_point);
                    }
                }
                if(pstMatch->astWeUav[i].nX == pstMatch->astWeUav[i].attack_target.x && pstMatch->astWeUav[i].nY == pstMatch->astWeUav[i].attack_target.y
                   && !(pstMatch->astWeUav[i].nX == pstMap->nParkingX && pstMatch->astWeUav[i].nY == pstMap->nParkingY)){
                    //pstMatch->astWeUav[i].route.clear();
                    if(pstMatch->astWeUav[i].nZ > pstMatch->astWeUav[i].attack_target.z){
                        Point3f tmp_point;
                        tmp_point.x = pstMatch->astWeUav[i].nX;
                        tmp_point.y = pstMatch->astWeUav[i].nY;
                        tmp_point.z = pstMatch->astWeUav[i].nZ - 1;
                        pstMatch->astWeUav[i].route.push_back (tmp_point);
                    }
                    if(pstMatch->astWeUav[i].nZ < pstMatch->astWeUav[i].attack_target.z){
                        Point3f tmp_point;
                        tmp_point.x = pstMatch->astWeUav[i].nX;
                        tmp_point.y = pstMatch->astWeUav[i].nY;
                        tmp_point.z = pstMatch->astWeUav[i].nZ + 1;
                        pstMatch->astWeUav[i].route.push_back (tmp_point);
                    }
                }
                if(!pstMatch->astWeUav[i].route.empty ()){
                    pstMatch->astWeUav[i].tag = 1;
                }
            }
        }

        for(int tmp_i=0;tmp_i<pstMatch->nUavWeNum;tmp_i++) { //遍历自己所有飞机
            int i = UAV_NO[tmp_i];
    //        pstMatch->astWeUav->nX==pstMap->nParkingX&&pstMatch->astWeUav->nY==pstMap->nParkingY //判断是否在停机坪
    //            &&pstMatch->astWeUav->nZ==0&&

            //判断被取物品是否异常

    //        cout << pstMatch->astWeUav[i].nNO << "- " << pstMatch->astWeUav[i].tag << "- " <<goodsno[pstMatch->astWeUav[i].target_goodsno].nState << "- " << pstMatch->astWeUav[i].nGoodsNo
    //             << "- " << pstMatch->astWeUav[i].nStatus << "- " << pstMatch->astWeUav[i].attack_flag << " - " << pstMatch->astWeUav[i].init_attack_flag << endl;
            if (pstMatch->astWeUav[i].tag == 1 && goodsno[pstMatch->astWeUav[i].target_goodsno].nState != 0 &&
                pstMatch->astWeUav[i].nGoodsNo == -1 //物品被取走了
                && pstMatch->astWeUav[i].nStatus == 0
                && pstMatch->astWeUav[i].attack_flag == 0
                && pstMatch->astWeUav[i].init_attack_flag == 0) {
                int keep_tag = 0;
                //cout << "gggggggggggggggggggggggggggggggggg" << endl;
                for (int j = 0; j < pstMatch->nUavEnemyNum; j++) {
                    if (pstMatch->astEnemyUav[j].nX == pstMatch->astWeUav[i].nX &&
                        pstMatch->astEnemyUav[j].nY == pstMatch->astWeUav[i].nY) {
                        keep_tag = 1;
                        pstMatch->astWeUav[i].target_goodsno = -1;
                        //cout << "kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk" << endl;
                    }
                }
                if (keep_tag == 0){
                    pstMatch->astWeUav[i].tag = 0;
                    goods_tag[pstMatch->astWeUav[i].target_goodsno] = 0;
                    pstMatch->astWeUav[i].route.clear();
                    vector<Point3f> best_path; //用于记录最好路径
                    double best_cost = 0.0;  //性价比
                    map<double, int> goodscost;
                    for (int j = 0; j < pstMatch->nGoodsNum; j++) {  //注意这里的货物编号不是从0开始顺序递增，而是随机编号，所以不要用j去表示编号
                        if (pstMatch->astWeUav[i].nLoadWeight >= pstMatch->astGoods[j].nWeight &&
                            goods_tag[pstMatch->astGoods[j].nNO] == 0  //tag表示这个物品有没有预订掉
                            && pstMatch->astGoods[j].nState == 0) { //物品没有被取掉
                            double tmp_cost = 0.0;
                            tmp_cost = CostFun(pstMatch->astGoods[j], pstMatch->astWeUav[i].nX,
                                               pstMatch->astWeUav[i].nY,pstMap->nHLow);  //计算性价比
                            if(tmp_cost>0) {
                                goodscost.insert(pair<double, int>(tmp_cost, j));
                            }
                        }
                    }
                    int judge_counts=0;
                    if (!goodscost.empty()) {
                        for (map<double, int>::reverse_iterator iter = goodscost.rbegin();
                             iter != goodscost.rend(); iter++) {
                            judge_counts++;
                            best_path = optimal_path(pstMap, &pstMatch->astWeUav[i], pstMatch->astGoods[iter->second]);
                            pstMatch->astWeUav[i].target_goodsno = pstMatch->astGoods[iter->second].nNO;
                            pstMatch->astWeUav[i].goods_startX = pstMatch->astGoods[iter->second].nStartX;
                            pstMatch->astWeUav[i].goods_startY = pstMatch->astGoods[iter->second].nStartY;
                            pstMatch->astWeUav[i].goods_endX = pstMatch->astGoods[iter->second].nEndX;
                            pstMatch->astWeUav[i].goods_endY = pstMatch->astGoods[iter->second].nEndY;
                            if (!best_path.empty() &&
                                pstMatch->astWeUav[i].go_stepNum < (pstMatch->astGoods[iter->second].nLeftTime - 6)) {
                              //  cout << i << "~~bei_quwuping_guihua" << endl;
                                pstMatch->astWeUav[i].route.assign(best_path.begin(), best_path.end());
                                goods_tag[pstMatch->astWeUav[i].target_goodsno] = 1;
                                pstMatch->astWeUav[i].tag = 1;
                                break;
                            }
                            if(judge_counts>5)
                                break;
                        }
                    }
                }
            }
    // 判断飞机状态 0为正常状态
            if (pstMatch->astWeUav[i].nStatus == 0 &&pstMatch->astWeUav[i].nGoodsNo==-1&& pstMatch->astWeUav[i].tag == 0 && pstMatch->astWeUav[i].route.empty()
                &&pstMatch->astWeUav[i].attack_flag==0 && pstMap->init_flag == -1 && pstMatch->astWeUav[i].init_attack_flag == 0) //判断有没有分配过物资编号，有没有进行过路径规划
            {
                vector<Point3f> best_path; //用于记录最好路径
                map<double, int> goodscost;
                for (int j = 0; j < pstMatch->nGoodsNum; j++) {  //注意这里的货物编号不是从0开始顺序递增，而是随机编号，所以不要用j去表示编号
                    if (pstMatch->astWeUav[i].nLoadWeight >= pstMatch->astGoods[j].nWeight &&
                        goods_tag[pstMatch->astGoods[j].nNO] == 0  //tag表示这个物品有没有预订掉
                        && pstMatch->astGoods[j].nState == 0) { //物品没有被取掉

                        double tmp_cost = 0.0;
                        tmp_cost = CostFun(pstMatch->astGoods[j], pstMatch->astWeUav[i].nX,
                                           pstMatch->astWeUav[i].nY,pstMap->nHLow);  //计算性价比
                        if(tmp_cost>0) {
                            goodscost.insert(pair<double, int>(tmp_cost, j));
                        }
                    }
                }
                int judge_counts=0;
                if (!goodscost.empty()) {
                    for(map<double,int>::reverse_iterator iter=goodscost.rbegin();iter!=goodscost.rend();iter++) {
                        judge_counts++;
                        best_path = optimal_path(pstMap, &pstMatch->astWeUav[i], pstMatch->astGoods[iter->second]);
                        pstMatch->astWeUav[i].target_goodsno = pstMatch->astGoods[iter->second].nNO;
                        pstMatch->astWeUav[i].goods_startX = pstMatch->astGoods[iter->second].nStartX;
                        pstMatch->astWeUav[i].goods_startY = pstMatch->astGoods[iter->second].nStartY;
                        pstMatch->astWeUav[i].goods_endX = pstMatch->astGoods[iter->second].nEndX;
                        pstMatch->astWeUav[i].goods_endY = pstMatch->astGoods[iter->second].nEndY;
                        if (!best_path.empty() &&
                            pstMatch->astWeUav[i].go_stepNum < (pstMatch->astGoods[iter->second].nLeftTime - 6)) {
                           // cout << i << "~~bei_zhengchang_guihua" << endl;
                            pstMatch->astWeUav[i].route.assign(best_path.begin(), best_path.end());
                            goods_tag[pstMatch->astWeUav[i].target_goodsno] = 1;
                            pstMatch->astWeUav[i].tag = 1;
                            break;
                        }
                        if(judge_counts>5)
                            break;
                    }
                    break;
    //            if(!best_path.empty())
    //            {
    //                pstMatch->astWeUav[i].route.assign(best_path.begin(),best_path.end());
    //                goods_tag[pstMatch->astWeUav[i].target_goodsno]=1;
    //                pstMatch->astWeUav[i].tag=1;
    ////                    for(int j=0;i<pstMatch->astWeUav[i].route.size ();j++)
    ////                    {
    ////                        cout << "plane x::" << pstMatch->astWeUav[i].route[j].x<< endl;
    ////                        cout << "plane y::" << pstMatch->astWeUav[i].route[j].y<< endl;
    ////                        cout << "plane z::" << pstMatch->astWeUav[i].route[j].z<< endl;
    ////                    }
    //                break;
    //            }

                }

            }else if(pstMatch->astWeUav[i].nStatus == 0 &&pstMatch->astWeUav[i].nGoodsNo==-1&& pstMatch->astWeUav[i].tag == 0
                     &&(pstMatch->astWeUav[i].attack_flag>0 || pstMatch->astWeUav[i].defend_flag != 0) && pstMatch->astWeUav[i].init_attack_flag == 0){        //攻击飞机规划

                vector<Point2f> planroute;
                Point3f raisepoint;
                for (int p = pstMatch->astWeUav[i].nZ; p <(pstMap->nHLow-1) ; ++p) {
                    raisepoint.x=pstMatch->astWeUav[i].nX;
                    raisepoint.y=pstMatch->astWeUav[i].nY;
                    raisepoint.z=(p+1);
                    pstMatch->astWeUav[i].route.push_back(raisepoint);
                }
                if (pstMatch->astWeUav[i].attack_flag<5 && pstMatch->astWeUav[i].attack_flag > 0){//中心攻击飞机
                    planroute=AstarAlgorithmFun (pstMap,pstMap->nHLow,pstMatch->astWeUav[i].nX,pstMatch->astWeUav[i].nY,VecRangeXY[pstMatch->astWeUav[i].attack_flag - 1].x,VecRangeXY[pstMatch->astWeUav[i].attack_flag - 1].y);
                }
                if (pstMatch->astWeUav[i].attack_flag==5) //停机坪攻击飞机
                {
                    planroute=AstarAlgorithmFun (pstMap,pstMap->nHLow,pstMatch->astWeUav[i].nX,pstMatch->astWeUav[i].nY,EnemyParkingX,EnemyParkingY);
                    pstMatch->astWeUav[i].attack_flag=-1;
                   // cout<<"~~~~~~~~~~~~~"<<endl;
                }
    //            if(pstMatch->astWeUav[i].defend_flag != 0){   //停机坪防守飞机
    //                for (int j = 0; j < pstMatch->nUavEnemyNum; ++j) {
    //                    if(abs(pstMatch->astEnemyUav[j].nX - pstMap->nParkingX) == 1 && abs(pstMatch->astEnemyUav[j].nY - pstMap->nParkingY) == 1){
    //                        planroute = AstarAlgorithmFun(pstMap, pstMap->nHLow, pstMatch->astWeUav[i].nX, pstMatch->astWeUav[i].nY, pstMatch->astEnemyUav[j].nX, pstMatch->astEnemyUav[j].nY);
    //                    }
    //                }
    //            }
                Point3f P3d;
                for (auto p:planroute) {
                    P3d.x=p.x;
                    P3d.y=p.y;
                    P3d.z=pstMap->nHLow;
                    pstMatch->astWeUav[i].route.push_back (P3d);

                }
                P3d.z = pstMap->nHLow - 1;
                pstMatch->astWeUav[i].route.push_back (P3d);
                pstMatch->astWeUav[i].tag=1;
              //  cout << i <<"~~bei_gongji_guihua" <<endl;
                break;
            }
            else if(pstMatch->astWeUav[i].nStatus == 0 &&pstMatch->astWeUav[i].nGoodsNo==-1&& pstMatch->astWeUav[i].tag == 0
                    && pstMatch->astWeUav[i].nZ < (pstMap->nHLow - 1) && pstMap->init_flag != -1&&pstMatch->astWeUav[i].attack_flag==0){ //当UAV状态良好，未载物，未规划路径，且z坐标小于最低飞行平面下一格时
                Point3f tmp_point;
                tmp_point.x = pstMatch->astWeUav[i].nX;
                tmp_point.y = pstMatch->astWeUav[i].nY;
                tmp_point.z = pstMatch->astWeUav[i].nZ;
                for(int n = 0; n < pstMap->init_flag; n ++){
                    pstMatch->astWeUav[i].route.push_back(tmp_point);
                }
                pstMap->init_flag++;
                while(tmp_point.z < pstMap->nHLow - pstMap->init_flag){
                    tmp_point.z += 1;
                    pstMatch->astWeUav[i].route.push_back(tmp_point);     //让该UAV上升一格
                }
              //  cout << i <<"~~bei_chushi_guihua" <<endl;
            }
            //判断最近货物并调用路径规划函数

        }
        pstMap->init_flag = -1;



        //更新可攻击飞机列表
        memset(attack_no, -1, sizeof(attack_no));
        for (int i = 0; i < pstMatch->nUavWeNum; i++) {
            if (pstMatch->astWeUav[i].nStatus == 0 && pstMatch->astWeUav[i].attack_flag == 1 ) {
                attack_no[0] = pstMatch->astWeUav[i].nNO;
            }
            if (pstMatch->astWeUav[i].nStatus == 0 && pstMatch->astWeUav[i].attack_flag == 2 ) {
                attack_no[1] = pstMatch->astWeUav[i].nNO;
            }
            if (pstMatch->astWeUav[i].nStatus == 0 && pstMatch->astWeUav[i].attack_flag == 3 ) {
                attack_no[2] = pstMatch->astWeUav[i].nNO;
            }
            if (pstMatch->astWeUav[i].nStatus == 0 && pstMatch->astWeUav[i].attack_flag == 4 ) {
                attack_no[3] = pstMatch->astWeUav[i].nNO;
            }
        }

        //检测区域可撞飞机
        for(int i=0;i<pstMatch->nUavEnemyNum;i++)
        {
            if(pstMatch->astEnemyUav[i].nZ<pstMap->nHLow && pstMatch->astEnemyUav[i].nGoodsNo==-1 && Enemy_attacktag[pstMatch->astEnemyUav[i].nNO]==0 && pstMatch->astEnemyUav[i].nStatus != 1)
            {
                for(int j=0;j<pstMatch->nGoodsNum;j++)
                {
                    if(pstMatch->astEnemyUav[i].nX==pstMatch->astGoods[j].nStartX && pstMatch->astEnemyUav[i].nY==pstMatch->astGoods[j].nStartY)
                    {
                        if((pstMatch->astGoods[j].nEndX<(pstMap->nMapX/2)) && (pstMatch->astGoods[j].nEndY<(pstMap->nMapY/2)) && attack_no[3] !=-1)
                        {
                            pstMatch->astWeUav[attack_no[3]].route.clear();
                            pstMatch->astWeUav[attack_no[3]].route=point_path (pstMap, &pstMatch->astWeUav[attack_no[3]], pstMatch->astGoods[j].nEndX,pstMatch->astGoods[j].nEndY);
                            pstMatch->astWeUav[attack_no[3]].attack_flag=0;
                            pstMatch->astWeUav[attack_no[3]].tag=1;
                            pstMatch->astWeUav[attack_no[3]].attack_no=pstMatch->astEnemyUav[i].nNO;
                            Enemy_attacktag[pstMatch->astEnemyUav[i].nNO]=1;
                            break;
                        }
                        else if((pstMatch->astGoods[j].nEndX>(pstMap->nMapX/2)) && (pstMatch->astGoods[j].nEndY<(pstMap->nMapY/2)) && attack_no[2] !=-1)
                        {
                            pstMatch->astWeUav[attack_no[2]].route.clear();
                            pstMatch->astWeUav[attack_no[2]].route=point_path (pstMap, &pstMatch->astWeUav[attack_no[2]], pstMatch->astGoods[j].nEndX,pstMatch->astGoods[j].nEndY);
                            pstMatch->astWeUav[attack_no[2]].attack_flag=0;
                            pstMatch->astWeUav[attack_no[2]].tag=1;
                            pstMatch->astWeUav[attack_no[2]].attack_no=pstMatch->astEnemyUav[i].nNO;
                            Enemy_attacktag[pstMatch->astEnemyUav[i].nNO]=1;
                            break;
                        }
                        else if((pstMatch->astGoods[j].nEndX>(pstMap->nMapX/2) ) && (pstMatch->astGoods[j].nEndY>(pstMap->nMapY/2)) && attack_no[1] !=-1)
                        {
                            pstMatch->astWeUav[attack_no[1]].route.clear();
                            pstMatch->astWeUav[attack_no[1]].route=point_path (pstMap, &pstMatch->astWeUav[attack_no[1]], pstMatch->astGoods[j].nEndX,pstMatch->astGoods[j].nEndY);
                            pstMatch->astWeUav[attack_no[1]].attack_flag=0;
                            pstMatch->astWeUav[attack_no[1]].tag=1;
                            pstMatch->astWeUav[attack_no[1]].attack_no=pstMatch->astEnemyUav[i].nNO;
                            Enemy_attacktag[pstMatch->astEnemyUav[i].nNO]=1;
                            break;
                        }
                        else if((pstMatch->astGoods[j].nEndX < (pstMap->nMapX/2)) && (pstMatch->astGoods[j].nEndY > (pstMap->nMapY/2) )&& attack_no[0] !=-1)
                        {
                            pstMatch->astWeUav[attack_no[0]].route.clear();
                            pstMatch->astWeUav[attack_no[0]].route=point_path (pstMap, &pstMatch->astWeUav[attack_no[0]], pstMatch->astGoods[j].nEndX,pstMatch->astGoods[j].nEndY);
                            pstMatch->astWeUav[attack_no[0]].attack_flag=0;
                            pstMatch->astWeUav[attack_no[0]].tag=1;
                            pstMatch->astWeUav[attack_no[0]].attack_no=pstMatch->astEnemyUav[i].nNO;
                            Enemy_attacktag[pstMatch->astEnemyUav[i].nNO]=1;
                            break;
                        }
                    }
                }

            }
        }
        //检测攻击飞机目标是否还存在
        for(int i=0;i<pstMatch->nUavWeNum;i++)
        {
            if(pstMatch->astWeUav[i].nStatus!=1 && (pstMatch->astWeUav[i].attack_no!=0 || pstMatch->astWeUav[i].init_attack_flag != 0)
               && Enemy_existtag[pstMatch->astWeUav[i].attack_no]==0)
            {
                    pstMatch->astWeUav[i].tag=0;
                    pstMatch->astWeUav[i].route.clear();
                    pstMatch->astWeUav[i].init_attack_flag = 0;      //该飞机不再是初始进攻飞机
            }
        }


        //购买无人机决策
        //平均性价比计算
        double unloadgoodsvalue=0;
        double bestvalue=0;
        int  best_no=0;
        int weight=0;
        const int attackuav_num=5;
        int now_attack_num=0;
        int Buy_list[5];
        memset (Buy_list, 0, sizeof (Buy_list));
        for (int k = 0; k <pstMatch->nUavWeNum ; ++k) {
            for (int i = 1; i <= 5; ++i) {
                if (pstMatch->astWeUav[k].attack_flag == i && pstMatch->astWeUav[k].nStatus != 1){
                    now_attack_num++;
                    Buy_list[i-1] = 1;
                }
            }
        }
      //  cout<<"@@@@@@@@"<<now_attack_num;
        // && pstMatch->astWeUav[i].nStatus != 1


        //性价比
        for (int i = 0; i <pstMatch->nGoodsNum ; ++i) {
            if(goods_tag[pstMatch->astGoods[i].nNO]==0) {
                int tag = 0;
                for (int j = 0; j < DONENO.size (); ++j) {
                    if (DONENO[i] == pstMatch->astGoods[i].nNO)
                        tag = 1;
                }
                if (tag == 0) {
                    goods_cost[pstMatch->astGoods[i].nNO] = CostFun (goodsno[pstMatch->astGoods[i].nNO], pstMap->nParkingX,
                                                                     pstMap->nParkingY,pstMap->nHLow);
                    if (goods_cost[pstMatch->astGoods[i].nNO] > bestvalue) {
                        bestvalue = goods_cost[pstMatch->astGoods[i].nNO];
                        best_no = pstMatch->astGoods[i].nNO;
                        weight = goodsno[best_no].nWeight;

                    }
                }
            }
        }
    //    for (int i = 0; i <pstMatch->nGoodsNum ; ++i) {
    //        if(goods_tag[pstMatch->astGoods[i].nNO]==0)
    //        {
    //            goods_cost[pstMatch->astGoods[i].nNO]= CostFun (goodsno[pstMatch->astGoods[i].nNO],pstMap->nParkingX,pstMap->nParkingY);
    //            if (goods_cost[pstMatch->astGoods[i].nNO]>bestvalue)
    //            {
    //                bestvalue=goods_cost[pstMatch->astGoods[i].nNO];
    //                best_no=pstMatch->astGoods[i].nNO;
    //                weight=goodsno[best_no].nWeight;
    //            }
    //        }
    //    }
    //    cout<<"dangqian_gongjifeiji_shumu"<<now_attack_num<<endl;
    //    cout<<"dangqian_feiji_shumu"<<pstMatch->nUavWeNum<<endl;

        int Weight_Difference=0;
        int Smallest_Difference=0;
        int PurNo=0;
        memset(pstFlayPlane->szPurchaseType, 0, sizeof (pstFlayPlane->szPurchaseType));
        pstFlayPlane->nPurchaseNum=0;


        int enableuavnum=0;
        for (int k1 = 0; k1 <pstMatch->nUavWeNum ; ++k1) {
            if(pstMatch->astWeUav[k1].nStatus!=1&&pstMatch->astWeUav[k1].attack_flag==0)
                enableuavnum++;
        }



bool FLAGA= true;
        if ( now_attack_num<attackuav_num&&enableuavnum>=8) {  //攻击飞机
            if (pstMatch->nWeValue >= pstMap->astUavPrice[cheapest_no].nValue)
            {


                for (int k = 0; k < 5; ++k) {
                    if(Buy_list[k] == 0){
                        Attack_Flag = k + 1;
                        //cout<<"AAAAAAAAAAAAAA"<<Attack_Flag<<endl;
                        break;
                    }
                }
                if(Attack_Flag==5&&AttParknum>=(pstMap->nHLow-2))
                {
                    FLAGA= false;
                }
                //                    cout<<"mai_le_jingong_feiji"<<pstMap->astUavPrice[cheapest_no].szType<<endl;
                if(FLAGA) {
                    pstFlayPlane->nPurchaseNum = 1;
                    strcpy (pstFlayPlane->szPurchaseType[0], pstMap->astUavPrice[cheapest_no].szType);
                    UAV_NO.push_back (pstMatch->nUavWeNum);
                }


            }
        }

    //    cout << "wwwwwwwwwwwwwwwwwwwwwwwwwwwww" << weight << endl;
        if(weight!=0) {
            Smallest_Difference=pstMap->astUavPrice[0].nLoadWeight-weight;
            for (int j = 0; j < pstMap->nUavPriceNum; ++j) {

                Weight_Difference= (pstMap->astUavPrice[j].nLoadWeight-weight);


                if (Weight_Difference>=0 &&
                    pstMatch->nWeValue >= pstMap->astUavPrice[j].nValue) {
                    if(Weight_Difference<Smallest_Difference)
                    {
                        PurNo=j;
                        Smallest_Difference=Weight_Difference;
                    }
                }

            }
            if( pstMatch->nWeValue >= pstMap->astUavPrice[PurNo].nValue&&pstFlayPlane->nPurchaseNum==0) {
                pstFlayPlane->nPurchaseNum = 1;
                DONENO.push_back(best_no);
                if(secure_flag){
                    PurNo = cheapest_no;
                    DONENO.pop_back();
                }
                strcpy (pstFlayPlane->szPurchaseType[0], pstMap->astUavPrice[PurNo].szType);
                UAV_NO.push_back (pstMatch->nUavWeNum);
    //            cout<<"mai_le_zhengchang_feiji"<<pstMap->astUavPrice[PurNo].szType<<endl;
            }
        }


        //输出飞机状态
        int enable_num=0;
        int x;
        int y;
        int z;

        //遍历每个飞机，把每个飞机的下一步指令传出去
        for(int i=0;i<pstMatch->nUavWeNum;i++)

        {
            //载货飞机送货点也敌方飞机堵截，停止飞行
            if(pstMatch->astWeUav[i].nGoodsNo != -1 && pstMatch->astWeUav[i].route.size() < pstMatch->astWeUav[i].nZ + 3 && pstMatch->astWeUav[i].route.size() > pstMap->nHLow){
                for (int j = 0; j < pstMatch->nUavEnemyNum; ++j) {
                    if(pstMatch->astEnemyUav[j].nX == pstMatch->astWeUav[i].goods_endX && pstMatch->astEnemyUav[j].nY == pstMatch->astWeUav[i].goods_endY && pstMatch->astEnemyUav[j].nZ <= pstMap->nHLow + 4){
                        pstMatch->astWeUav[i].defend_flag_counts++;
                        if(pstMatch->astWeUav[i].defend_flag_counts > 2){
                            pstMatch->astWeUav[i].route.clear();
                            pstMatch->astWeUav[i].defend_flag_counts = 0;
                        }
                        break;
                    } else{
                        pstMatch->astWeUav[i].defend_flag_counts = 0;
                    }
                }
            }
            int intersect_flag = 0;      //交换位置
            int seize_flag = 0;        //抢占位置

    //        if(pstMatch->astWeUav[i].nStatus!=1)
    //        {
            enable_num++;
            pstFlayPlane->astUav[i]=pstMatch->astWeUav[i]; //不动



            if(!(pstMatch->astWeUav[i].route.empty())) {
                //是否有以前的飞机抢占位置
                if((pstFlayPlane->astUav[i].nX != pstMap->nParkingX && pstFlayPlane->astUav[i].nY != pstMap->nParkingY) || pstMatch->astWeUav[i].nZ >= pstMap->nHLow){
                    for(int j = 0; j < i; j++){
                        if(pstFlayPlane->astUav[j].nX == pstMatch->astWeUav[i].route[0].x && pstFlayPlane->astUav[j].nY == pstMatch->astWeUav[i].route[0].y && pstFlayPlane->astUav[j].nZ == pstMatch->astWeUav[i].route[0].z)
                            seize_flag = 1;
                    }
                    //是否会和之后的飞机交换位置
                    for(int j = i + 1; j < pstMatch->nUavWeNum; j++){
                        if(!(pstMatch->astWeUav[j].route.empty ())){
                            if(pstFlayPlane->astUav[i].nX == pstMatch->astWeUav[j].route[0].x
                               && pstFlayPlane->astUav[i].nY == pstMatch->astWeUav[j].route[0].y && pstFlayPlane->astUav[i].nZ == pstMatch->astWeUav[j].route[0].z
                               && pstMatch->astWeUav[i].route[0].x == pstFlayPlane->astUav[j].nX
                               && pstMatch->astWeUav[i].route[0].y == pstFlayPlane->astUav[j].nY && pstMatch->astWeUav[i].route[0].z == pstFlayPlane->astUav[j].nZ)
                                intersect_flag = 1;
                        }
                        //同平面，且同在x或y轴，另一坐标相差一，在下一时刻另一坐标互换    5-27
                        if(pstMatch->astWeUav[j].nStatus!=1&&pstFlayPlane->astUav[i].nZ == pstFlayPlane->astUav[j].nZ && !(pstMatch->astWeUav[j].route.empty ())
                           && ((pstFlayPlane->astUav[i].nY == pstFlayPlane->astUav[j].nY && abs(pstFlayPlane->astUav[i].nX - pstFlayPlane->astUav[j].nX) == 1 && pstMatch->astWeUav[i].route[0].x == pstFlayPlane->astUav[j].nX && pstMatch->astWeUav[j].route[0].x == pstFlayPlane->astUav[i].nX)
                           || (pstFlayPlane->astUav[i].nX == pstFlayPlane->astUav[j].nX && abs(pstFlayPlane->astUav[i].nY - pstFlayPlane->astUav[j].nY) == 1 && pstMatch->astWeUav[i].route[0].y == pstFlayPlane->astUav[j].nY && pstMatch->astWeUav[j].route[0].y == pstFlayPlane->astUav[i].nY)))
                            intersect_flag = 1;
                    }

                }
//                if(pstMatch->astWeUav[i].attack_flag == -1){
//                    seize_flag = 0;
//                }
                //for (auto p:pstMatch->astWeUav[0].route)
                // cout<<"321231asdasda"<<p.z<<endl;
                if (!intersect_flag && !seize_flag&&pstFlayPlane->astUav[i].nStatus!=1) {
                    pstFlayPlane->astUav[i].nX = pstMatch->astWeUav[i].route[0].x;
                    pstFlayPlane->astUav[i].nY = pstMatch->astWeUav[i].route[0].y;
                    pstFlayPlane->astUav[i].nZ = pstMatch->astWeUav[i].route[0].z;
                    pstMatch->astWeUav[i].route.erase(pstMatch->astWeUav[i].route.begin ());
                }
                else if((intersect_flag || seize_flag)){    //将交叉碰撞
                    if(pstFlayPlane->astUav[i].nZ != pstMap->nHHigh){       //不在最高飞行高度上
                        Point3f tmp_point;
                        tmp_point.x = pstFlayPlane->astUav[i].nX;
                        tmp_point.y = pstFlayPlane->astUav[i].nY;
                        tmp_point.z = pstFlayPlane->astUav[i].nZ;
                        pstFlayPlane->astUav[i].nZ += 1;    //该UAV上升一个高度
                        pstMatch->astWeUav[i].route.insert( pstMatch->astWeUav[i].route.begin(), tmp_point);
                    }else{                                                  //在最高飞行高度上
                        //该UAV下降一个高度 , 然后在下一时刻上升回来
                        Point3f tmp_point;
                        tmp_point.x = pstFlayPlane->astUav[i].nX;
                        tmp_point.y = pstFlayPlane->astUav[i].nY;
                        tmp_point.z = pstFlayPlane->astUav[i].nZ;
                        pstFlayPlane->astUav[i].nZ -= 1;
                        pstMatch->astWeUav[i].route.insert( pstMatch->astWeUav[i].route.begin(), tmp_point);
                    }
                }


            }


            if(pstFlayPlane->astUav[i].nX==pstMatch->astWeUav[i].goods_startX
               && pstFlayPlane->astUav[i].nY==pstMatch->astWeUav[i].goods_startY
               && pstFlayPlane->astUav[i].nZ==0)
            {
                for(int zq=0;zq<pstMatch->nGoodsNum;zq++)
                {
                    if(pstMatch->astGoods[zq].nNO==pstMatch->astWeUav[i].target_goodsno && pstMatch->astGoods[zq].nState==0)
                    {
                        pstFlayPlane->astUav[i].nGoodsNo=pstMatch->astWeUav[i].target_goodsno;
                        break;
                    }
                }
    //            cout << "**********************************************************" << endl;//装载物品
//                pstFlayPlane->astUav[i].nGoodsNo=pstMatch->astWeUav[i].target_goodsno;
    //                cout<<"asdlalallsdlsakdkasd"<<pstFlayPlane->astUav[i].target_goodsno<<endl;
    //                return;
            }
            if(pstFlayPlane->astUav[i].nX==pstMatch->astWeUav[i].goods_endX
               && pstFlayPlane->astUav[i].nY==pstMatch->astWeUav[i].goods_endY
               && pstFlayPlane->astUav[i].nZ==0)
            {                       //卸载物品
                pstMatch->astWeUav[i].tag=0;
                goods_tag[pstMatch->astWeUav[i].target_goodsno]=0;
                buyuav=true;

            }




    //        }
        }






        pstFlayPlane->nUavNum=enable_num;

    //    for (int m1 = 0; m1 <pstFlayPlane->nUavNum ; ++m1) {
    //        cout << "NO " << pstFlayPlane->astUav[m1].nNO << " x: "
    //             << pstFlayPlane->astUav[m1].nX << " y: " << pstFlayPlane->astUav[m1].nY
    //             <<" z: " << pstFlayPlane->astUav[m1].nZ << " nstatus " << pstFlayPlane->astUav[m1].nStatus
    //             << " goodsno " <<pstFlayPlane->astUav[m1].nGoodsNo<< endl;
    //        cout<<m1<<"!!!!!!!!!!!!"<<pstMatch->astWeUav[m1].szType<<endl;
    //    }





    }



    //**********************代码*************************//
    int main(int argc, char *argv[])
    {
    #ifdef OS_WINDOWS
        // windows下，需要进行初始化操作
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        {
            printf("WSAStartup failed\n");
            return false;
        }
    //    for (int m1 = 0; m1 <pstFlayPlane->nUavNum ; ++m1) {
    //        cout << "NO " << pstFlayPlane->astUav[m1].nNO << " x: "
    //             << pstFlayPlane->astUav[m1].nX << " y: " << pstFlayPlane->astUav[m1].nY
    //             <<" z: " << pstFlayPlane->astUav[m1].nZ << " nstatus " << pstFlayPlane->astUav[m1].nStatus
    //             << " goodsno " <<pstFlayPlane->astUav[m1].nGoodsNo<< endl;
    //        cout<<m1<<"!!!!!!!!!!!!"<<pstMatch->astWeUav[m1].szType<<endl;
    //    }
    #endif

        char        szIp[64] = { 0 };
        int         nPort = 0;
        char        szToken[128] = { 0 };
        int         nRet = 0;
        OS_SOCKET   hSocket;
        char        *pRecvBuffer = NULL;
        char        *pSendBuffer = NULL;
        int         nLen = 0;

        // 本地调试去掉这个
        if (argc != 4)
        {
            printf("error arg num\n");
            return -1;
        }

        // 解析参数
        strcpy(szIp, argv[1]);
        nPort = atoi(argv[2]);
        strcpy(szToken, argv[3]);

    //    strcpy(szIp, "39.105.71.189");
    //    nPort = 30176;
    //    strcpy(szToken, "44c65540-93fc-4792-b7d6-d9fbe73be9f5");

        printf("server ip %s, prot %d, token %s\n", szIp, nPort, szToken);

        // 开始连接服务器
        nRet = OSCreateSocket(szIp, (unsigned int)nPort, &hSocket);
        if (nRet != 0)
        {
            printf("connect server error\n");
            return nRet;
        }

        // 分配接受发送内存
        pRecvBuffer = (char*)malloc(MAX_SOCKET_BUFFER);
        if (pRecvBuffer == NULL)
        {
            return -1;
        }

        pSendBuffer = (char*)malloc(MAX_SOCKET_BUFFER);
        if (pSendBuffer == NULL)
        {
            free(pRecvBuffer);
            return -1;
        }

        memset(pRecvBuffer, 0, MAX_SOCKET_BUFFER);

        // 接受数据  连接成功后，Judger会返回一条消息：
        nRet = RecvJuderData(hSocket, pRecvBuffer);
        if (nRet != 0)
        {
            return nRet;
        }

        // json 解析
        // 获取头部
        CONNECT_NOTICE  stNotice;

        nRet = ParserConnect(pRecvBuffer + SOCKET_HEAD_LEN, &stNotice);
        if (nRet != 0)
        {
            return nRet;
        }

        // 生成表明身份的json
        TOKEN_INFO  stToken;

        strcpy(stToken.szToken, szToken);  // 如果是调试阶段，请输入你调试的token，在我的对战中获取，
        // 实际比赛，不要传入调试的，按照demo写的，有服务器调用传入。
        strcpy(stToken.szAction, "sendtoken");

        memset(pSendBuffer, 0, MAX_SOCKET_BUFFER);
        nRet = CreateTokenInfo(&stToken, pSendBuffer, &nLen);
        if (nRet != 0)
        {
            return nRet;
        }
        // 选手向裁判服务器表明身份(Player -> Judger)
        nRet = SendJuderData(hSocket, pSendBuffer, nLen);
        if (nRet != 0)
        {
            return nRet;
        }

        //身份验证结果(Judger -> Player)　
        memset(pRecvBuffer, 0, MAX_SOCKET_BUFFER);
        nRet = RecvJuderData(hSocket, pRecvBuffer);
        if (nRet != 0)
        {
            return nRet;
        }

        // 解析验证结果的json
        TOKEN_RESULT      stResult;
        nRet = ParserTokenResult(pRecvBuffer + SOCKET_HEAD_LEN, &stResult);
        if (nRet != 0)
        {
            return 0;
        }

        // 是否验证成功
        if (stResult.nResult != 0)
        {
            printf("token check error\n");
            return -1;
        }
        // 选手向裁判服务器表明自己已准备就绪(Player -> Judger)
        READY_PARAM     stReady;

        strcpy(stReady.szToken, szToken);
        strcpy(stReady.szAction, "ready");

        memset(pSendBuffer, 0, MAX_SOCKET_BUFFER);
        nRet = CreateReadyParam(&stReady, pSendBuffer, &nLen);
        if (nRet != 0)
        {
            return nRet;
        }
        nRet = SendJuderData(hSocket, pSendBuffer, nLen);
        if (nRet != 0)
        {
            return nRet;
        }

        //对战开始通知(Judger -> Player)　
        memset(pRecvBuffer, 0, MAX_SOCKET_BUFFER);
        nRet = RecvJuderData(hSocket, pRecvBuffer);
        if (nRet != 0)
        {
            return nRet;
        }
        // 解析数据
        //Mapinfo 结构体可能很大，不太适合放在栈中，可以定义为全局或者内存分配
        MAP_INFO            *pstMapInfo;
        MATCH_STATUS        *pstMatchStatus;
        FLAY_PLANE          *pstFlayPlane;

        pstMapInfo = (MAP_INFO *)malloc(sizeof(MAP_INFO));
        if (pstMapInfo == NULL)
        {
            return -1;
        }

        pstMatchStatus = (MATCH_STATUS *)malloc(sizeof(MATCH_STATUS));
        if (pstMapInfo == NULL)
        {
            return -1;
        }

        pstFlayPlane = (FLAY_PLANE *)malloc(sizeof(FLAY_PLANE));
        if (pstFlayPlane == NULL)
        {
            return -1;
        }

        memset(pstMapInfo, 0, sizeof(MAP_INFO));
        memset(pstMatchStatus, 0, sizeof(MATCH_STATUS));
        memset(pstFlayPlane, 0, sizeof(FLAY_PLANE));

        nRet = ParserMapInfo(pRecvBuffer + SOCKET_HEAD_LEN, pstMapInfo);
        if (nRet != 0)
        {
            return nRet;
        }

        // 第一次把无人机的初始赋值给flayplane
        pstFlayPlane->nPurchaseNum = 0;
        pstFlayPlane->nUavNum = pstMapInfo->nUavNum;
        for (int i = 0; i < pstMapInfo->nUavNum; i++)
        {
            pstFlayPlane->astUav[i] = pstMapInfo->astUav[i];
        }

        // 根据服务器指令，不停的接受发送数据
        while (1)
        {
            //chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
            // 进行当前时刻的数据计算, 填充飞行计划结构体，注意：0时刻不能进行移动，即第一次进入该循环时
            if (pstMatchStatus->nTime != 0)
            {
                AlgorithmCalculationFun(pstMapInfo, pstMatchStatus, pstFlayPlane);
            }


            //发送飞行计划结构体
            memset(pSendBuffer, 0, MAX_SOCKET_BUFFER);
            nRet = CreateFlayPlane(pstFlayPlane, szToken, pSendBuffer, &nLen);
            if (nRet != 0)
            {
                return nRet;
            }
            nRet = SendJuderData(hSocket, pSendBuffer, nLen);
            if (nRet != 0)
            {
                return nRet;
            }

            printf("%s\n", pSendBuffer);

            // 接受当前比赛状态
            memset(pRecvBuffer, 0, MAX_SOCKET_BUFFER);
            nRet = RecvJuderData(hSocket, pRecvBuffer);
            if (nRet != 0)
            {
                return nRet;
            }

            // 解析
            nRet = ParserMatchStatus(pRecvBuffer + SOCKET_HEAD_LEN, pstMatchStatus);
            if (nRet != 0)
            {
                return nRet;
            }

            if (pstMatchStatus->nMacthStatus == 1)
            {
                // 比赛结束
                printf("game over, we value %d, enemy value %d\n", pstMatchStatus->nWeValue, pstMatchStatus->nEnemyValue);
                return 0;
            }
            //chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
           // chrono::duration<double> time_used = chrono::duration_cast<chrono::duration<double>>( t2-t1 );
           // cout<<"solve time cost = "<<time_used.count()<<" seconds. "<<endl;
        }

        // 关闭socket
        OSCloseSocket(hSocket);
        // 资源回收
        free(pRecvBuffer);
        free(pSendBuffer);
        free(pstMapInfo);
        free(pstMatchStatus);
        free(pstFlayPlane);

        return 0;
    }




