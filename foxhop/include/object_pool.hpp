#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <process.h>
#include <memory.h>

/**
    @brief  ������Ʈ�� �̸� �����ص� Ǯ
    @remark �� Ǯ�� ������� ������Ʈ Ŭ������
    @n      pNext, pPrev, bActivate ����� �����ؾ��Ѵ�.
    @n      ���� ������ ������ ����� Init �ż��嵵 �����ؾ��Ѵ�.
    @n      ������Ʈ Ŭ�������� �ڵ� Ŀ�ø��� �����ؾ��Ѵ�.
*/
template <typename T> class ObjectPool {
private:
    T*   pInactivate;      /**< ����ִ� ������Ʈ ������*/
    T*   pActivate;        /**< �������� ������Ʈ ������*/
    T*   pObject;          /**< Ǯ*/
    BOOL bAssigned;        /**< �� Ǯ�� ��뿩��. (�ܵ����� �ʿ���� �� Ǯ�� ���� Ŭ������ ���)*/
    int  nAdditionalCnt;   /**< �ִ�뷮 �ʰ��� �߰��Ҵ�� ���� ����*/
    int  nCapacity;        /**< Ǯ���� �ִ� ������Ʈ ����*/
    int  nUsingCnt;        /**< ���� Ǯ���� ������� ������Ʈ ����*/
    int  nUsingMemoryByte; /**< ���� Ǯ�� �޸� ��뷮*/
    
public:
    ObjectPool();
    ~ObjectPool();
    BOOL Init(int nCnt = 250, int nAddCnt = 250);
    void Release();
    void Reset();
    BOOL isAssigned();
    void setAssigned(BOOL bFlag);
    int  getCapacity();
    int  getUsingCount();
    int  getUsingMemoryBytes();
    T*   getActiveHead();
    T*   activateObject();
    void deactivateObject(T* pObj);
};

/**
    @brief  ������ƮǮ�� �̸� �����Ͽ� �߱����ִ� ������ �Ŵ���
    @remark Ǯ�� �ʿ��ϸ� ���⼭ �̸� �ʱ�ȭ�� Ǯ�� �߱�/�ݳ��Ѵ�.
    @n      ���ø� T���� ������Ʈ�� ����.
    @n      ex) ObjectPoolList<ObjectMotionBox>
*/
template <typename T> class ObjectPoolList {

private:
    ObjectPool<T>** pPoolList;       /**< �������� ������ Ǯ ����Ʈ (�ʱ�ȭ �̷��� �������� ����)*/
    int            nCapacity;        /**< ��ü Ǯ ����*/
    int            nUsingCnt;        /**< ���� ������� Ǯ ����*/
    int            nTotalPoolsSize;  /**< ���� Ǯ ��ü�� �޸� ��뷮*/
    int            nTotalUsingSize;  /**< ���� �Ҵ�� ��� ������Ʈ ũ�� ����*/
public:
    ObjectPoolList();
    ~ObjectPoolList();
    BOOL           Init(int nPoolCnt, int nObjCnt);
    ObjectPool<T>* getPool();
    void           returnPool(ObjectPool<T>* pPool);
};

template <typename T> ObjectPool<T>::ObjectPool()
{
    pInactivate = 0;
    pActivate = 0;
    pObject = 0;
    bAssigned = FALSE;
    nAdditionalCnt = 0;
    nCapacity = 0;
    nUsingCnt = 0;
    nUsingMemoryByte = 0;
}

template <typename T> ObjectPool<T>::~ObjectPool() {}

/**
    @brief Ǯ�� �ʱ�ȭ �ϰ� ��ĭ ����Ʈ�� �̾���´�.
*/
template <typename T> BOOL ObjectPool<T>::Init(int nCnt, int nAddCnt)
{
    int i;

#if 0 /*����� ��� ũ��Ȯ�� ��� On Off*/
#if _DEBUG /*����� ��� �����, ������Ʈ ũ�Ⱑ �ʹ� ũ�� ������ �˸��� ���*/
#define LIMIT_SIZE (256)
    {
        static BOOL f = 0;
        if (sizeof(T) > LIMIT_SIZE && !f) {
            MessageBoxA(NULL, "Object size is a little big (ObjectPool Initialize routine)", "Info", MB_OK);
            f = TRUE;
        }
    }
#undef LIMIT_SIZE
#endif
#endif

    nUsingMemoryByte = sizeof(T) * nCnt;    /*���� Ŭ���� ũ�� x �Ҵ簹��*/
    pObject = (T*)malloc(nUsingMemoryByte);
    if (!pObject) return FALSE;

    /*������Ʈ �ʱ�ȭ ����*/
    for (i = 0; i < nCnt; i++) {
        pObject[i].pPrev = &pObject[i - 1];
        pObject[i].pNext = &pObject[i + 1]; /*���Ḯ��Ʈ ����*/
        pObject[i].bActivate = FALSE;
    }
    pObject[0].pPrev = NULL;                /*�� ù��° ���Ҵ� ������带 �����*/
    pObject[i - 1].pNext = NULL;            /*�� ������ ���Ҵ� ������带 �����*/

    pInactivate = &pObject[0];              /*ù��° ������Ʈ�� ����ִ� ������Ʈ�� ���*/
    pActivate = NULL;                       /*���� Ȱ��ȭ�� ������Ʈ ����*/
    nAdditionalCnt = nAddCnt;               /*TODO : ���߿� ������ �� �� ����*/
    nCapacity = nCnt;
    nUsingCnt = 0;
    bAssigned = FALSE;                      /*���� ��������� ���� Ǯ*/
    return TRUE;
}

/**
    @brief Ǯ ����
*/
template <typename T> void ObjectPool<T>::Release()
{
    if (pObject) free((void*)pObject);
}

/**
    @brief  �̹� ������� Ǯ�� �����Ѵ�
    @remark �̹� �Ҵ�� �޸𸮴� �״�� ����ϵ�
    @n      �� ���Ḯ��Ʈ�� �籸���Ѵ�
*/
template <typename T> void ObjectPool<T>::Reset()
{
    int i;

    memset((void*)pObject, 0, nUsingMemoryByte);
    for (i = 0; i < nCapacity; i++) {
        pObject[i].pPrev = &pObject[i - 1];
        pObject[i].pNext = &pObject[i + 1]; /*���Ḯ��Ʈ ����*/
    }
    pObject[0].pPrev = NULL;                /*�� ù��° ���Ҵ� ������带 �����*/
    pObject[i - 1].pNext = NULL;            /*�� ������ ���Ҵ� ������带 �����*/
}

/**
    @brief  ���� Ǯ�� ��� ���θ� ���´�
    @remark �� �ż���� �� Ǯ�� �̿��ϴ� ���� Ŭ������ ���Ѱ���
    @n      �Ϲ������δ� ��� �� ���� ����, ����ص� �ƹ����� ����.
*/
template <typename T> BOOL ObjectPool<T>::isAssigned()
{
    return bAssigned;
}

/**
    @brief  ���� Ǯ�� ��� ���θ� �����Ѵ�
    @remark �� �ż���� �� Ǯ�� �̿��ϴ� ���� Ŭ������ ���Ѱ���
    @n      �� Ǯ�� �ܺο��� ���� �����ǰ� �ִٸ�, �ǵ��� ����
*/
template <typename T> void ObjectPool<T>::setAssigned(BOOL bFlag)
{
    bAssigned = bFlag;
}


/**
    @brief ���� Ǯ�� �ִ� �ѵ��� ���´�
*/
template <typename T> int ObjectPool<T>::getCapacity()
{
    return nCapacity;
}

/**
    @brief ���� ������� ������Ʈ ���� ���´�
*/
template <typename T> int ObjectPool<T>::getUsingCount()
{
    return nUsingCnt;
}

/**
    @brief ��Ƹ��� �޸𸮸� ��ȯ�Ѵ�
*/
template <typename T> int ObjectPool<T>::getUsingMemoryBytes()
{
    return nUsingMemoryByte;
}

/**
    @brief  Ȱ��ȭ�� ������Ʈ�� ���� �� ��带 �����´�
    @remark ������Ʈ���� �۾� ������ ��� ������ �ϰ�ó�������� ���.
    @n      ������ �߿��� ��, ���ε��� ó�� �ؾ���
*/
template <typename T> T* ObjectPool<T>::getActiveHead()
{
    return pActivate;
}

/**
    @brief  Ǯ���� ������Ʈ�� Ȱ��ȭ ��Ų��
    @remark ���� ������Ʈ�� ������ Ǯ�� Ȯ���Ѵ� (���� realloc�� ���װ� �־� Ȯ�� X)
*/
template <typename T> T* ObjectPool<T>::activateObject()
{
    T* pTmp;

    if (!pObject) return NULL;
    if (!pInactivate) { /*��� �ִ� ������Ʈ ������ Ǯ Ȯ��*/
#if 0 /* TODO : realloc �� �����Ͱ� �ٲ�� ��� ��ũ�帮��Ʈ�� ������. */
        nUsingMemoryByte += sizeof(T) * nAdditionalCnt;
        pObject = (T*)realloc((void*)pObject, nUsingMemoryByte);
        if (!pObject) {
            MessageBoxA(NULL, "ObjectPool realloc Fail", "Fatal Error", MB_OK);
            exit(-1); /*���̻� �������� ������ ������ �� �����Ƿ� ���α׷� ���� ����*/
            return -1;
        }
        nCapacity += nAdditionalCnt; /*�ִ� �뷮 ����*/
        for (i = nUsingCnt; i < nCapacity; i++) {
            pObject[i].pPrev = &pObject[i - 1]; /*���Ḯ��Ʈ �籸��*/
            pObject[i].pNext = &pObject[i + 1];
            pObject[i].bActivate = FALSE;
        }
        pObject[nUsingCnt].pPrev = NULL; /*������ ���°� ��� ����������̱� ������ ������´�*/
        pObject[i - 1].pNext = NULL;
        pInactivate = &pObject[nUsingCnt]; /*�籸��� ������Ʈ ��ũ ������*/
#else
        return NULL; /*���װ� ���� �� �� ������ Ȯ�� ����.*/
#endif
    }
    /*��Ȱ��ȭ ����Ʈ���� ����*/
    pTmp = pInactivate;
    pInactivate = pInactivate->pNext;   /*����ִ� ������Ʈ ����Ʈ���� �߶󳽴�*/
    memset(pTmp, 0, sizeof(T));         /*�߱� �� �ʱ�ȭ*/
    pTmp->bActivate = TRUE;             /*Ȱ��ȭ �÷��� ON*/

    /*Ȱ��ȭ ����Ʈ�� ü��*/
    pTmp->pNext = pActivate;
    pActivate = pTmp;                   /*ü��*/
    pActivate->pPrev = NULL;            /*�� ���� ���� ���濡 �����̹Ƿ� ������尡 ����.*/
    if (pActivate->pNext) pActivate->pNext->pPrev = pActivate; /*�� �� ����� �ٷ� ���� ��� prev ������ ü��*/
    nUsingCnt++;
    return pTmp; /*����ϰ� �ʱ�ȭ�� ������Ʈ ��ȯ*/
}

/**
    @brief  ������ ������Ʈ�� Ǯ�� ��ȯ
*/
template <typename T> void ObjectPool<T>::deactivateObject(T* pObj)
{
    T* pTmp;

    if (!nUsingCnt) return;       /*�̹� ���� ��ȯ�Ǿ��ִ� ��� �� ��û�� �����Ѵ�*/
    if (!pObj->bActivate) return; /*�̹� ��Ȱ��ȭ �Ǿ��ִ� ��� �� ��û�� ����*/

    /*Ȱ��ȭ ����Ʈ���� ����*/
    if (!pObj->pPrev) /*��Ȱ��ȭ �Ϸ��� ��尡 �Ӹ� ����ΰ��*/
        pActivate = pObj->pNext; /*���� ��带 �Ӹ� ���� ���*/

    else if (!pObj->pNext) /*��Ȱ��ȭ �Ϸ��� ��尡 ��Ʈ�Ӹ� ����ΰ��*/
        pObj->pPrev->pNext = NULL; /*���� ����� pNext �� ����ش�*/

    else { /*��Ȱ��ȭ �Ϸ��� ��尡 �߰��� ���� ����� ���*/
        pObj->pPrev->pNext = pObj->pNext;
        pObj->pNext->pPrev = pObj->pPrev; /*�ճ�� �޳�带 ���� ü��*/
    }

    /*��Ȱ��ȭ ����Ʈ�� ����*/
    pTmp = pInactivate;             /*��Ȱ��ȭ ����Ʈ �Ӹ� ��� ������ (NULL�ϼ��� ����)*/
    pInactivate = pObj;             /*��Ȱ��ȭ ����Ʈ �Ӹ� ��� ����*/
    pInactivate->pNext = pTmp;      /*ü�� �Ϸ�*/
    pInactivate->bActivate = FALSE; /*Ȱ��ȭ �÷��� OFF*/
    nUsingCnt--;
}

template <typename T> ObjectPoolList<T>::ObjectPoolList()
{
    pPoolList = 0;
    nCapacity = 0;
    nUsingCnt = 0;
    nTotalPoolsSize = 0;
    nTotalUsingSize = 0;
}

template <typename T> ObjectPoolList<T>::~ObjectPoolList() {}

/**
    @brief ������ƮǮ�� ������ ���� �߱����ִ� ������ �Ŵ���
*/
template <typename T> BOOL ObjectPoolList<T>::Init(int nPoolCnt, int nObjCnt)
{
    int i;

    nTotalPoolsSize = sizeof(ObjectPool<T>) * nPoolCnt;
    nCapacity = nPoolCnt;
    pPoolList = (ObjectPool<T>**)malloc(nTotalPoolsSize);
    if (!pPoolList) return FALSE;

    /*Ǯ ����Ʈ ������ �̸� �ʱ�ȭ �صα�*/
    for (i = 0; i < nPoolCnt; i++) {
        pPoolList[i] = new ObjectPool<T>;
        pPoolList[i]->Init(nObjCnt);
    }
    nTotalUsingSize = pPoolList[0]->getUsingMemoryBytes() * nPoolCnt;
    nUsingCnt = 0; /*���� ������� Ǯ ���� 0���� �ʱ�ȭ*/
    return TRUE;
}

/**
    @brief ������ Ǯ ����Ʈ���� Ǯ�� �ϳ� ���´�.
*/
template <typename T> ObjectPool<T>* ObjectPoolList<T>::getPool()
{
    ObjectPool<T>* p;

    if (nUsingCnt >= nCapacity) return NULL; /*���� Ǯ�� ������ NULL*/
    p = pPoolList[nUsingCnt];
    p->Reset();                              /*Ǯ ��ü ���Ḯ��Ʈ �籸��*/
    p->setAssigned(TRUE);                    /*Ǯ ��� �÷��� ON*/
    nUsingCnt++;
    return p;
}

/**
    @brief Ǯ ����Ʈ�� Ǯ�� ��ȯ�Ѵ�.
*/
template <typename T> void ObjectPoolList<T>::returnPool(ObjectPool<T>* pPool)
{
    if (nUsingCnt <= 0) return;       /*�̹� ���� ��ȯ ���¶�� �� ��û�� ���õȴ�*/
    if (!pPool->isAssigned()) return; /*�̹� ��Ȱ��ȭ�� Ǯ�� ��ȯ���� ����*/
    nUsingCnt--;
    pPool->setAssigned(FALSE);        /*Ǯ ��� �÷��� OFF*/
    pPoolList[nUsingCnt] = pPool;     /*Ǯ ��ȯ*/
}