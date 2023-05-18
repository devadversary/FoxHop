#pragma once

#include "windows.h"
#define MAX_TREE_DEPTH (256)             /*최대 트리 깊이*/
#define TREENODE_ALLOCED_NOUSERDATA ((void*)-1)  /*트리의 노드에 유저데이터가 들어있지 않음*/

/**
    @brief 트리 노드 구조체
*/
typedef struct _node_tree
{
    BYTE               value;   /**< 탐색용 표식*/
    struct _node_tree* pSub;    /**< 하위 트리노드*/
    struct _node_tree* pNext;   /**< 같은 레벨의 다음 트리노드*/
    void*              pParam;  /**< 추가 등록 정보*/
}NODE_TREE;

/**
    @brief 트리 객체
*/
typedef struct _tree
{
    NODE_TREE* pRoot;     /**< 루트노드*/
    int        nElements; /**< 들어간 데이터 갯수*/
    /*TODO : 메모리풀 안에서 트리가 생성되도록 개선해야 함. (삭제시 유리)*/
}TREE;

/**
    @brief 트리 순회 상황 열거자 (사용자가 직접 다룰 일은 없음)
*/
typedef enum _e_TreeEnum
{
    eTreeEnum_Start = 0, /**< 순회 작업 안 한 상태*/
    eTreeEnum_Sub,       /**< 하위 노드 순회 해야함*/
    eTreeEnum_Next,      /**< 다음 노드 순회 해야함*/
    eTreeEnum_End        /**< 순회 완료 상태*/
}eTreeEnum;

/**
    @brief 트리 순회시 사용할 순회 기록표
*/
typedef struct _TreeEnum
{
    NODE_TREE* pNode;     /**< 순회중인 노드*/
    eTreeEnum  EnumState; /**< 순회 상태*/
}TREE_ENUM;

/**
    @brief 트리 순회시, 데이터 찾을때마다 실행할 함수 콜백
*/
typedef void(*pfnTreeEnum)(NODE_TREE* pNode, void* pParam);

/**
    @brief 트리 노드 초기화
    @return 할당 실패시 NULL
*/
BOOL TREE_Init(TREE* pTree);

/**
    @brief 트리에 새 데이터를 넣는다.
    @return 삽입된 노드의 포인터
    @remark pData가 NULL 이라면, 노드의 pParam는 TREENODE_ALLOCED_NOUSERDATA 로 초기화 된다.
    @n      중복 데이터의경우 새로 삽입 되지 않고 NULL 을 반환한다.
*/
NODE_TREE* TREE_Input(TREE* pTree, void* pKey, int nKeyLen, void* pData);

/**
    @brief 트리에서 데이터를 삭제한다.
    @return 삭제되면 TRUE, 삭제할 데이터가 없으면 FALSE
    @remark 트리에 등록된 데이터도(pParam)  같이 해제된다.
*/
BOOL TREE_Delete(TREE* pTree, void* pKey, int nKeyLen);

/**
    @brief 트리 검색 함수
*/
NODE_TREE* TREE_Search(TREE* pTree, void* pKey, int nKeyLen);

/**
    @brief 트리 순회
    @param pTree          순회할 트리의 핸들
    @param pfnCallback    순회중 데이터 발견시 호출할 콜백 (필요 없으면 NULL)
    @param pCallbackParam 콜백에 전달할 인자
    @return 발견한 데이터 갯수
*/
int TREE_Enumerate(TREE* pTree, pfnTreeEnum pfnCallback, void* pCallbackParam);
