#include "tree.hpp"

static void SAFE_FREE(void* p)
{
    if (!p) return;
    if (p == (void*)-1) return;
    free(p);
}

/**
    @brief Ʈ�� ��� �ʱ�ȭ
    @return �Ҵ� ���н� NULL
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
    @brief Ʈ���� �� �����͸� �ִ´�.
    @return ���Ե� ����� ������
    @remark pData�� NULL �̶��, ����� pParam�� -1�� �ʱ�ȭ �ȴ�.
    @n      �ߺ� �������ǰ�� ���� ���� ���� �ʰ� NULL �� ��ȯ�Ѵ�.
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
        /*���� ǥ��*/
        if (pTmp->value == pSrc[nDepth]) {
            /*���� ����*/
            if (nDepth == nKeyLen - 1) {
                if (pTmp->pParam) return NULL; /*�̷��� �Ǹ� �ߺ��̹Ƿ� ���� ��ȯ*/
                pTmp->pParam = pData ? pData : TREENODE_ALLOCED_NOUSERDATA; /* 0���� ����θ� �ȵ�.*/
                pTree->nElements++;
                return pTmp;
            }

            /*�ٸ� ����*/
            if (!pTmp->pSub) {
                pTmp->pSub = (NODE_TREE*)calloc(1, sizeof(NODE_TREE));
                pTmp->pSub->value = pSrc[nDepth+1]; /*�� ������� ��� �� �ʱ�ȭ*/
            }
            pTmp = pTmp->pSub;
            nDepth++;
            continue; /*���� ����*/
        }

        /*�ٸ� ǥ��*/
        if(!pTmp->pNext) {
            pTmp->pNext = (NODE_TREE*)calloc(1, sizeof(NODE_TREE));
            pTmp->pNext->value = pSrc[nDepth]; /*�� ������� ��� �� �ʱ�ȭ*/
        }
        pTmp = pTmp->pNext; /*���� ����*/
    }
    return NULL;
}

/**
    @brief Ʈ�� ���� ���꿡���� ����� ����ü
    @remark �� ����ü�� ������Ͽ� �������� �ʴ´�.
*/
typedef struct _st_tree_deletion_node
{
    NODE_TREE* pPrev; /**< pCur�� ���� ��� (�θ��尡 �ƴ�)*/
    NODE_TREE* pCur;  /**< ���� ���*/
}TREE_DEL;

/**
    @brief Ʈ������ �����͸� �����Ѵ�.
    @return �����Ǹ� TRUE, ������ �����Ͱ� ������ FALSE
    @remark Ʈ���� ��ϵ� �����͵�(pParam)  ���� �����ȴ�.
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
        /*���� ǥ��*/
        if (pTmp->value == pSrc[nDepth]) {
            /*���� ���� - ��ǥ ��� ����*/
            NodeTrace[nDepth].pPrev = pTmpPrev;
            NodeTrace[nDepth].pCur  = pTmp;
            if (nDepth == nKeyLen - 1) {
                if (pTmp->pParam) break; /*���� ��� �����͵� ������ �ùٸ��� ã�� ��*/
                return FALSE; /*�̹� param�� ������ ��� (����ġ�� ��������?)*/
            }

            pTmp = pTmp->pSub;
            nDepth++;
            pTmpPrev = 0; /*���� ��� �湮 ��, ������� ǥ�� �ʱ�ȭ.*/
            continue;  /*���� ����*/
        }

        /*�ٸ� ǥ��*/
        NodeTrace[nDepth].pPrev = pTmpPrev = pTmp;
        NodeTrace[nDepth].pCur = pTmp->pNext;
        pTmp = pTmp->pNext; /*���� ����*/
    }

    if (!pTmp) return FALSE; /*��� ��ã��*/

    /*pParam �� 0 �ƴϰ�, TREENODE_ALLOCED_NOUSERDATA �� �ƴҶ��� ���� �Ҵ�� ������*/
    pTrace = &NodeTrace[nDepth];
    if (pTrace->pCur->pParam && pTrace->pCur->pParam != TREENODE_ALLOCED_NOUSERDATA)
        SAFE_FREE(pTrace->pCur->pParam); /*���� ������ ������ 1ȸ�� �Ͼ��.*/

    pTree->nElements--; /*�� �Ʒ��� ���� return TRUE �̹Ƿ�, ���� ������ ����ȴ�.*/
    while(nDepth >= 0) {
        pTrace = &NodeTrace[nDepth];

        /*�Ʒ� ��� Ȯ��*/
        if (pTrace->pCur->pSub){
            return TRUE; /*������ ���⼭ ���� (��� ������ X)*/
        }

        /*���� ��� Ȯ��*/
        if (pTrace->pPrev){
            pTrace->pPrev->pNext = pTrace->pCur->pNext; /*������� pNext�� ������ pNext�� ü��*/
            SAFE_FREE(pTrace->pCur); /*���� ���� ����*/
            return TRUE; /*��������*/
        }

        /*���� ��� Ȯ��*/
        if (pTrace->pCur->pNext){
            /*�θ����� pSub�� �������� Next�� ü�� (�迭 ���� ���� ����)*/
            if (nDepth) NodeTrace[nDepth-1].pCur->pSub = pTrace->pCur->pNext;
            SAFE_FREE(pTrace->pCur); /*���� ���� ����*/
            return TRUE; /*��������*/
        }

        if (pTrace->pCur->pParam) return TRUE; /*���� �����Ͱ� ������ ���⼭ ���� ����*/

        /*�Ʒ�, ����, ���� ��� ��� ������ (�迭 ���� ���� ����)*/
        SAFE_FREE(pTrace->pCur); /*���� ��� ����*/
        if (nDepth) NodeTrace[nDepth-1].pCur->pSub = 0;
        nDepth--;
    }

    return TRUE; 
}

/**
    @brief Ʈ�� �˻� �Լ�
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
        /*���� ǥ��*/
        if (pTmp->value == pSrc[nDepth]) {
            /*���� ���� - ��ǥ ��� ����*/
            if (nDepth == nKeyLen - 1) {
                if (pTmp->pParam) return pTmp; /*���� ��� �����͵� ������, ����*/
                return NULL;
            }

            /*�ٸ� ����*/
            pTmp = pTmp->pSub;
            nDepth++;
            continue; /*���� ����*/
        }

        /*�ٸ� ǥ��*/
        pTmp = pTmp->pNext; /*���� ����*/
    }
    return NULL; /*�� ã��*/
}

/**
    @brief Ʈ�� ��ȸ
    @param pTree          ��ȸ�� Ʈ���� �ڵ�
    @param pfnCallback    ��ȸ�� ������ �߽߰� ȣ���� �ݹ� (�ʿ� ������ NULL)
    @param pCallbackParam �ݹ鿡 ������ ����
    @return �߰��� ������ ����
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
        case eTreeEnum_Start: /*���۽�, ������ Ȯ�κ���.*/
            pTmpEnum->EnumState = eTreeEnum_Sub; /*������ ������ �湮*/
            if (pTmpEnum->pNode->pParam) {/*������ �߽߰�, ó�� �ݹ� ȣ��*/
                if (pfnCallback) pfnCallback(pTmpEnum->pNode, pCallbackParam);
                nCount++; /*�߰��� ������ ī��Ʈ*/
            }
            continue; /*while continue*/

        case eTreeEnum_Sub: /*������ �湮�� ���ÿ� ��� �� ���� ����*/
            pTmpEnum->EnumState = eTreeEnum_Next; /*������ �� ��� �湮*/
            if (pTmpEnum->pNode->pSub) {
                nDepth++;
                EnumNode[nDepth].pNode = pTmpEnum->pNode->pSub; /*���� ���� ���*/
                EnumNode[nDepth].EnumState = eTreeEnum_Start;
            }
            continue; /*while continue*/

        case eTreeEnum_Next: /*���� ��� �湮�� ���� ���� (�������� X)*/
            pTmpEnum->EnumState = eTreeEnum_End; /*������ ��*/
            if (pTmpEnum->pNode->pNext) {
                EnumNode[nDepth].pNode = pTmpEnum->pNode->pNext; /*���� ���� ���*/
                EnumNode[nDepth].EnumState = eTreeEnum_Start;
            }
            continue; /*while continue*/

        case eTreeEnum_End: /*�۾� ��. ���� ����*/
            nDepth--;
            continue; /*while continue*/
        }
    }
    return nCount;
}