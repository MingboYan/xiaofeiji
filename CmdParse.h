/**	@file       CmdParse.h
 *	@note       Hikvision Digital Technology Co., Ltd. All Right Reserved.
 *	@brief
 *
 *	@author     lipengfei
 *	@date       2018/05/10
 *	@note       ��ʷ��¼��
 *	@note       V1.0.0
 *	@warning
 */
#ifndef __CMDPARSE_H__
#define __CMDPARSE_H__

#include "JsonParse.h"
#include "iostream"
#include "vector"


#define SOCKET_HEAD_LEN          8                      ///< 8���ֽڵ�ͷ������
using namespace std;

typedef struct _CONNECT_NOTICE_
{
    char    szNotice[64];
    char    szMsg[128];
}CONNECT_NOTICE;

typedef struct _TOKEN_INFO_
{
    char    szToken[64];
    char    szAction[64];
}TOKEN_INFO;


typedef struct _TOKEN_RESULT_
{
    char    szToken[64];
    char    szNotice[64];
    char    szRoundId[64];
    char    szPalyerId[64];
    int     nResult;
}TOKEN_RESULT;

typedef struct _READY_PARAM_
{
    char    szToken[64];
    char    szAction[64];
}READY_PARAM;



typedef struct _BUILDING_
{
    int     nX;
    int     nY;
    int     nL;
    int     nW;
    int     nH;
}BUILDING;

typedef struct _FOG_
{
    int     nX;
    int     nY;
    int     nL;
    int     nW;
    int     nB;
    int     nT;
}FOG;

typedef struct _Point3f_
{
    int x;
    int y;
    int z;
}Point3f;

typedef struct _Point2f_
{
    int x;
    int y;
}Point2f;


#define MAX_BUILDING_NUM        128
#define MAX_FOG_NUM        128

#define MAX_UAV_NUM         512

#define MAX_UAV_PRICE_NUM    64

#define MAX_GOODS_NUM       256



typedef enum _UAV_STATUS_
{
    UAV_NOMAL = 0,
    UAV_CRASH,
    UAV_FOG
}UAV_STATUS;

typedef struct _UAV_
{
    int     nNO;
    char    szType[8];
    int     nX;
    int     nY;
    int     nZ;
    int     nLoadWeight;            ///< ��type��Ӧ�����˻�������һ����
    UAV_STATUS  nStatus;
    int     nGoodsNo;
    int     target_goodsno;
    int     tag;
    vector<Point3f> route;
    int     fly_height;
    int     load_height;
    int     go_stepNum;
    int     goods_startX;
    int     goods_startY;
    int     goods_endX;
    int     goods_endY;
    int     attack_flag;
    int     defend_flag_counts = 0;    //送货防御
    int     defend_flag = 0;                //停机坪防御
    int     init_attack_flag;
    //int     init_attack_target;
    Point3f attack_target;
    int     attack_no;
}UAV;


typedef struct _UAV_PRICE_
{
    char    szType[8];
    int     nLoadWeight;
    int     nValue;
}UAV_PRICE;

/** @struct
 * 	@brief
 *	@note
 */
typedef struct _MAP_INFO_
{
    int     nMapX;
    int     nMapY;
    int     nMapZ;
    int     nParkingX;
    int     nParkingY;
    int     init_flag = 0;

    int     nHLow;
    int     nHHigh;
    int     nBuildingNum;
    BUILDING    astBuilding[MAX_BUILDING_NUM];
    int     nFogNum;
    FOG         astFog[MAX_FOG_NUM];
    int     nUavNum;
    UAV     astUav[MAX_UAV_NUM];
    int     nUavPriceNum;
    UAV_PRICE   astUavPrice[MAX_UAV_PRICE_NUM];
}MAP_INFO;


typedef struct _FLAY_PLANE_
{
    int     nUavNum;
    UAV     astUav[MAX_UAV_NUM];

    int     nPurchaseNum;
    char    szPurchaseType[MAX_UAV_PRICE_NUM][8];
}FLAY_PLANE;

typedef struct _GOODS_
{
    int     nNO;
    int     nStartX;
    int     nStartY;
    int     nEndX;
    int     nEndY;
    int     nWeight;
    int     nValue;
    int     nStartTime;
    int     nRemainTime;
    int     nLeftTime;
    int     nState;
//    bool    isCan_fetch = true;

}GOODS;

typedef struct _MATCH_STATUS_
{
    int     nTime; //当前时间: 当前的时间，每次给比赛者都会比上一次增加1
    int     nMacthStatus;  //比赛状态: 0表示正常比赛中，1表示比赛结束，收到为1时，参赛者可以关闭连接,
    int     nUavWeNum; //我方的飞机数目
    UAV     astWeUav[MAX_UAV_NUM];
    int     nWeValue;//我方价值
    int     nUavEnemyNum;//敌方的飞机数目
    UAV     astEnemyUav[MAX_UAV_NUM];
    int     nEnemyValue;
    int     nGoodsNum;
    GOODS   astGoods[MAX_GOODS_NUM];
}MATCH_STATUS;

/** @fn     int ParserConnect(char *pBuffer, CONNECT_NOTICE *pstNotice)
 *  @brief
 *	@param  -I   - char * pBuffer
 *	@param  -I   - CONNECT_NOTICE * pstNotice
 *	@return int
 */
int ParserConnect(char *pBuffer, CONNECT_NOTICE *pstNotice);


/** @fn     int ParserTokenResult(char *pBuffer, TOKEN_RESULT *pResult)
 *  @brief
 *	@param  -I   - char * pBuffer
 *	@param  -I   - TOKEN_RESULT * pResult
 *	@return int
 */
int ParserTokenResult(char *pBuffer, TOKEN_RESULT *pResult);


/** @fn     int ParserMapInfo(char *pBuffer, MAP_INFO *pstMap)
 *  @brief
 *	@param  -I   - char * pBuffer
 *	@param  -I   - MAP_INFO * pstMap
 *	@return int
 */
int ParserMapInfo(char *pBuffer, MAP_INFO *pstMap);


/** @fn     int ParserUav(cJSON *pUavArray, UAV *astUav, int *pNum)
 *  @brief
 *	@param  -I   - cJSON * pUavArray
 *	@param  -I   - UAV * astUav
 *	@param  -I   - int * pNum
 *	@return int
 */
int ParserUav(cJSON *pUavArray, UAV *astUav, int *pNum);

/** @fn     int ParserMatchStatus(char *pBuffer, MATCH_STATUS *pstStatus)
 *  @brief
 *	@param  -I   - char * pBuffer
 *	@param  -I   - MATCH_STATUS * pstStatus
 *	@return int
 */
int ParserMatchStatus(char *pBuffer, MATCH_STATUS *pstStatus);


/** @fn     int CreateTokenInfo(TOKEN_INFO *pstInfo, char *pBuffer)
 *  @brief
 *	@param  -I   - TOKEN_INFO * pstInfo
 *	@param  -I   - char * pBuffer
 *	@return int
 */
int CreateTokenInfo(TOKEN_INFO *pstInfo, char *pBuffer, int *pLen);

/** @fn     int CreateReadyParam(READY_PARAM *pstParam, char *pBuffer, int *pLen)
 *  @brief
 *	@param  -I   - READY_PARAM * pstParam
 *	@param  -I   - char * pBuffer
 *	@param  -I   - int * pLen
 *	@return int
 */
int CreateReadyParam(READY_PARAM *pstParam, char *pBuffer, int *pLen);


/** @fn     int CreateFlayPlane(FLAY_PLANE *pstPlane, char *pBuffer, int *pLen)
 *  @brief
 *	@param  -I   - FLAY_PLANE * pstPlane
 *	@param  -I   - char * pBuffer
 *	@param  -I   - int * pLen
 *	@return int
 */
int CreateFlayPlane(FLAY_PLANE *pstPlane, char *szToken, char *pBuffer, int *pLen);

#endif

