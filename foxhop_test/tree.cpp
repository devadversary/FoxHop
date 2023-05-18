#include "tree.hpp"

static void SAFE_FREE(void* p)
{
    if (!p) return;
    if (p == (void*)-1) return;
    free(p);
}

/**
    @brief 트리 노드 초기화
    @return 할당 실패시 NULL
*/
BOOL TREE_Init(TREE* pTree)
{
    if (!pTree) return FALSE;
    memset(pTree, 0, sizeof(TREE));
    pTree->pRoot = (NODE_TREE*)calloc(1, sizeof(NODE_TREE));
    if (!pTree->pRoot) return FALSE;
    return TRUE;
}

/**
    @brief 트리에 새 데이터를 넣는다.
    @return 삽입된 노드의 포인터
    @remark pData가 NULL 이라면, 노드의 pParam는 -1로 초기화 된다.
    @n      중복 데이터의경우 새로 삽입 되지 않고 NULL 을 반환한다.
*/
NODE_TREE* TREE_Input(TREE* pTree, void* pKey, int nKeyLen, void* pData)
{
    BYTE*      pSrc;
    NODE_TREE* pTmp;
    int        nDepth = 0;

    if ( !pTree || !pTree->pRoot || nKeyLen <= 0) return NULL;
    if (nKeyLen > MAX_TREE_DEPTH) return NULL;

    pSrc = (BYTE*)pKey;
    pTmp = pTree->pRoot;
    while (1) {
        /*같은 표식*/
        if (pTmp->value == pSrc[nDepth]) {
            /*같은 길이*/
            if (nDepth == nKeyLen - 1) {
                if (pTmp->pParam) return NULL; /*이렇게 되면 중복이므로 오류 반환*/
                pTmp->pParam = pData ? pData : TREENODE_ALLOCED_NOUSERDATA; /* 0으로 비워두면 안됨.*/
                pTree->nElements++;
                return pTmp;
            }

            /*다른 길이*/
            if (!pTmp->pSub) {
                pTmp->pSub = (NODE_TREE*)calloc(1, sizeof(NODE_TREE));
                pTmp->pSub->value = pSrc[nDepth+1]; /*새 하위노드 등록 및 초기화*/
            }
            pTmp = pTmp->pSub;
            nDepth++;
            continue; /*하위 노드로*/
        }

        /*다른 표식*/
        if(!pTmp->pNext) {
            pTmp->pNext = (NODE_TREE*)calloc(1, sizeof(NODE_TREE));
            pTmp->pNext->value = pSrc[nDepth]; /*새 다음노드 등록 및 초기화*/
        }
        pTmp = pTmp->pNext; /*다음 노드로*/
    }
    return NULL;
}

/**
    @brief 트리 삭제 연산에서만 사용할 구조체
    @remark 이 구조체는 헤더파일에 노출하지 않는다.
*/
typedef struct _st_tree_deletion_node
{
    NODE_TREE* pPrev; /**< pCur의 이전 노드 (부모노드가 아님)*/
    NODE_TREE* pCur;  /**< 현재 노드*/
}TREE_DEL;

/**
    @brief 트리에서 데이터를 삭제한다.
    @return 삭제되면 TRUE, 삭제할 데이터가 없으면 FALSE
    @remark 트리에 등록된 데이터도(pParam)  같이 해제된다.
*/
BOOL TREE_Delete(TREE* pTree, void* pKey, int nKeyLen)
{
    TREE_DEL  NodeTrace[MAX_TREE_DEPTH];
    TREE_DEL* pTrace;
    BYTE* pSrc;
    NODE_TREE* pTmp;
    NODE_TREE* pTmpPrev  = 0;
    int        nDepth = 0;

    if (!pTree || !pTree->pRoot || !pKey || nKeyLen <= 0) return FALSE;

    pSrc = (BYTE*)pKey;
    pTmp = pTree->pRoot;
    pTmpPrev = 0;
    while (pTmp) {
        /*같은 표식*/
        if (pTmp->value == pSrc[nDepth]) {
            /*같은 길이 - 목표 노드 도달*/
            NodeTrace[nDepth].pPrev = pTmpPrev;
            NodeTrace[nDepth].pCur  = pTmp;
            if (nDepth == nKeyLen - 1) {
                if (pTmp->pParam) break; /*실제 등록 데이터도 있으면 올바르게 찾은 것*/
                return FALSE; /*이미 param이 지워진 노드 (가지치기 할지말지?)*/
            }

            pTmp = pTmp->pSub;
            nDepth++;
            pTmpPrev = 0; /*하위 노드 방문 전, 이전노드 표기 초기화.*/
            continue;  /*하위 노드로*/
        }

        /*다른 표식*/
        NodeTrace[nDepth].pPrev = pTmpPrev = pTmp;
        NodeTrace[nDepth].pCur = pTmp->pNext;
        pTmp = pTmp->pNext; /*다음 노드로*/
    }

    if (!pTmp) return FALSE; /*노드 못찾음*/

    /*pParam 이 0 아니고, TREENODE_ALLOCED_NOUSERDATA 도 아닐때는 실제 할당된 데이터*/
    pTrace = &NodeTrace[nDepth];
    if (pTrace->pCur->pParam && pTrace->pCur->pParam != TREENODE_ALLOCED_NOUSERDATA)
        SAFE_FREE(pTrace->pCur->pParam); /*유저 데이터 삭제는 1회만 일어난다.*/

    pTree->nElements--; /*이 아래는 전부 return TRUE 이므로, 삭제 성공은 보장된다.*/
    while(nDepth >= 0) {
        pTrace = &NodeTrace[nDepth];

        /*아래 노드 확인*/
        if (pTrace->pCur->pSub){
            return TRUE; /*있으면 여기서 종료 (노드 삭제는 X)*/
        }

        /*이전 노드 확인*/
        if (pTrace->pPrev){
            pTrace->pPrev->pNext = pTrace->pCur->pNext; /*이전노드 pNext를 현재노드 pNext와 체결*/
            SAFE_FREE(pTrace->pCur); /*현재 노드는 삭제*/
            return TRUE; /*연산종료*/
        }

        /*다음 노드 확인*/
        if (pTrace->pCur->pNext){
            /*부모노드의 pSub와 현재노드의 Next와 체결 (배열 음수 참조 방지)*/
            if (nDepth) NodeTrace[nDepth-1].pCur->pSub = pTrace->pCur->pNext;
            SAFE_FREE(pTrace->pCur); /*현재 노드는 삭제*/
            return TRUE; /*연산종료*/
        }

        if (pTrace->pCur->pParam) return TRUE; /*유저 데이터가 있으면 여기서 연산 종료*/

        /*아래, 이전, 이후 노드 모두 없으면 (배열 음수 참조 방지)*/
        SAFE_FREE(pTrace->pCur); /*현재 노드 삭제*/
        if (nDepth) NodeTrace[nDepth-1].pCur->pSub = 0;
        nDepth--;
    }

    return TRUE; 
}

/**
    @brief 트리 검색 함수
*/
NODE_TREE* TREE_Search(TREE* pTree, void* pKey, int nKeyLen)
{
    BYTE*      pSrc;
    NODE_TREE* pTmp;
    int        nDepth = 0;

    if (!pTree || !pTree->pRoot || !pKey || nKeyLen <= 0) return NULL;
    pSrc = (BYTE*)pKey;
    pTmp = pTree->pRoot;
    while (pTmp) {
        /*같은 표식*/
        if (pTmp->value == pSrc[nDepth]) {
            /*같은 길이 - 목표 노드 도달*/
            if (nDepth == nKeyLen - 1) {
                if (pTmp->pParam) return pTmp; /*실제 등록 데이터도 있으면, 성공*/
                return NULL;
            }

            /*다른 길이*/
            pTmp = pTmp->pSub;
            nDepth++;
            continue; /*하위 노드로*/
        }

        /*다른 표식*/
        pTmp = pTmp->pNext; /*다음 노드로*/
    }
    return NULL; /*못 찾음*/
}

/**
    @brief 트리 순회
    @param pTree          순회할 트리의 핸들
    @param pfnCallback    순회중 데이터 발견시 호출할 콜백 (필요 없으면 NULL)
    @param pCallbackParam 콜백에 전달할 인자
    @return 발견한 데이터 갯수
*/
int TREE_Enumerate(TREE* pTree, pfnTreeEnum pfnCallback, void* pCallbackParam)
{
    TREE_ENUM  EnumNode[MAX_TREE_DEPTH];
    TREE_ENUM* pTmpEnum;
    int        nDepth, nCount;

    if (!pTree || !pTree->pRoot) return 0;

    memset(EnumNode, 0, sizeof(EnumNode));
    nDepth = 0; nCount = 0;
    EnumNode[0].pNode = pTree->pRoot;
    EnumNode[0].EnumState = eTreeEnum_Start;

    while (EnumNode[0].EnumState != eTreeEnum_End) {
        pTmpEnum = &EnumNode[nDepth];
        switch (pTmpEnum->EnumState) {
        case eTreeEnum_Start: /*시작시, 데이터 확인부터.*/
            pTmpEnum->EnumState = eTreeEnum_Sub; /*다음은 서브노드 방문*/
            if (pTmpEnum->pNode->pParam) {/*데이터 발견시, 처리 콜백 호출*/
                if (pfnCallback) pfnCallback(pTmpEnum->pNode, pCallbackParam);
                nCount++; /*발견한 데이터 카운트*/
            }
            continue; /*while continue*/

        case eTreeEnum_Sub: /*서브노드 방문시 스택에 기록 후 깊이 증가*/
            pTmpEnum->EnumState = eTreeEnum_Next; /*다음은 옆 노드 방문*/
            if (pTmpEnum->pNode->pSub) {
                nDepth++;
                EnumNode[nDepth].pNode = pTmpEnum->pNode->pSub; /*하위 스택 기록*/
                EnumNode[nDepth].EnumState = eTreeEnum_Start;
            }
            continue; /*while continue*/

        case eTreeEnum_Next: /*다음 노드 방문시 스택 갱신 (깊이증가 X)*/
            pTmpEnum->EnumState = eTreeEnum_End; /*다음은 끝*/
            if (pTmpEnum->pNode->pNext) {
                EnumNode[nDepth].pNode = pTmpEnum->pNode->pNext; /*하위 스택 기록*/
                EnumNode[nDepth].EnumState = eTreeEnum_Start;
            }
            continue; /*while continue*/

        case eTreeEnum_End: /*작업 끝. 깊이 감소*/
            nDepth--;
            continue; /*while continue*/
        }
    }
    return nCount;
}