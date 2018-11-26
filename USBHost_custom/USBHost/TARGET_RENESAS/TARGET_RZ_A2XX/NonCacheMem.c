/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer*
* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/******************************************************************************
* @file         util.c
* $Rev: 456 $
* $Date:: 2014-07-08 19:47:01 +0900#$
* @brief        Utilities
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/***************************************************************************
 * Platform Includes
 ***************************************************************************/
#include <string.h>

/***************************************************************************
 * SDK Includes
 ***************************************************************************/
#include "mbed_rtos_storage.h"
#include "rtx_lib.h"
#include "NonCacheMem.h"

/***************************************************************************
 * Application Includes
 ***************************************************************************/

/* Gather following contents by the component unit of the object. */
/******************************************************************************
Macro definitions
******************************************************************************/
#define MPL_TBL_MAX             (1L)

#define CHK_ALIGN_4             (0x00000003uL)
#define UTIL_MTX_WAIT_TIME      (0xFFFFFFFFuL)
#define ADJOINING_PREV          (0x00000001uL)
#define ADJOINING_NEXT          (0x00000002uL)

/* error codes */
#define R_UTIL_RET_OK              (0L)
#define R_UTIL_RET_ERROR_RESOURCE  (-1L)
#define R_UTIL_RET_ERROR_ID        (-2L)
#define R_UTIL_RET_ERROR_ISR       (-3L)
#define R_UTIL_RET_ERROR_TIMEOUT   (-4L)
#define R_UTIL_RET_ERROR_INIT      (-5L)
#define R_UTIL_RET_ERROR_PARAMETER (-6L)
#define R_UTIL_RET_ERROR_PAR       (-7L)

/******************************************************************************
Typedef definitions
******************************************************************************/
struct mem_struct
{
    struct mem_struct *     sp_next;
    uint32_t                size;
};
typedef struct mem_struct mem_st;

typedef struct
{
    osMutexId_t             pool_mtx_id;
    uint32_t                total_size;
    mem_st *                p_top_mem;
    mem_st *                p_free_list;
    mem_st *                p_use_list;
} mpl_st;

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
/* OS resource definition */
static mbed_rtos_storage_mutex_t _mtx_obj_mem[MPL_TBL_MAX];

/* Memory definition */
static mpl_st sa_mpl[MPL_TBL_MAX] = {
    {NULL, 0, NULL, NULL, NULL}
};

static int32_t mem_id = -1;

static int32_t R_UTIL_CreatePool(uint32_t const mpl_size, void * const p_mpl);
//static int32_t R_UTIL_DeletePool(int32_t const pool_id);
static int32_t R_UTIL_GetPool(int32_t const pool_id, uint32_t const get_size, void ** const pp_get_mem);
static int32_t R_UTIL_RelPool(int32_t const pool_id, void * const p_rel_mem);

void * AllocNonCacheMem(uint32_t len) {
    static uint8_t non_cache_buf[1024 * 32] __attribute((section("NC_BSS"),aligned(4)));  //4 bytes aligned!
    void * p_get_mem;

    if (mem_id < 0) {
        mem_id = R_UTIL_CreatePool(sizeof(non_cache_buf), (void *)non_cache_buf);
    }
    if (R_UTIL_GetPool(mem_id, len, &p_get_mem) != R_UTIL_RET_OK) {
        p_get_mem = NULL;
    }
    return p_get_mem;
}

void FreeNonCacheMem(void * buf) {
    R_UTIL_RelPool(mem_id, buf);
}

/**************************************************************************//**
* Function Name: R_UTIL_CreatePool<br>
* @brief         Memory pool create processing<br>
*
*                Description:<br>
*                Process that returns the Memory pool control number of unused
* @param[in]     mpl_size        :Size of the variable-size memory pool area (in bytes).
* @param[in]     p_mpl           :Start address of the variable-size memory pool area.
* @retval        R_UTIL_RET_OK             :Success.
* @retval        R_UTIL_RET_ERROR_RESOURCE :Maximum resource count.
* @retval        R_UTIL_RET_ERROR_ISR      :This API cannot be called from interrupt service routines.
* @retval        R_UTIL_RET_ERROR_INIT     :Initialization isn't complete.
* @retval        R_UTIL_RET_ERROR_PARAMETER:Parameter error.
******************************************************************************/
static int32_t R_UTIL_CreatePool(uint32_t const mpl_size, void * const p_mpl)
{
    int32_t result;

    if ((p_mpl == NULL) || (mpl_size <= sizeof(mem_st))
     || (((uint32_t)p_mpl & CHK_ALIGN_4) != 0uL) || ((mpl_size & CHK_ALIGN_4) != 0uL))
    {
        result = R_UTIL_RET_ERROR_PARAMETER;
    }
    else
    {
        int32_t  index;

        result = R_UTIL_RET_ERROR_RESOURCE;
        for (index = 0L; index < MPL_TBL_MAX; index++)
        {
            mpl_st * const sp_mpl = &sa_mpl[index];

            if (sp_mpl->pool_mtx_id == NULL)
            {
                osMutexAttr_t attr = { 0 };
                memset(&_mtx_obj_mem[index], 0, sizeof(os_mutex_t));
                attr.name = "alloc_non_cache_mem_mutex";
                attr.cb_mem = &_mtx_obj_mem[index];
                attr.cb_size = sizeof(os_mutex_t);
                attr.attr_bits = osMutexRecursive | osMutexPrioInherit | osMutexRobust;

                osMutexId_t work_mtx_id = osMutexNew(&attr);

                if (work_mtx_id != NULL)
                {
                    mem_st * const sp_mem = (mem_st *)p_mpl;

                    sp_mem->sp_next         = NULL;
                    sp_mem->size            = mpl_size;
                    sp_mpl->total_size      = mpl_size;
                    sp_mpl->p_top_mem       = sp_mem;
                    sp_mpl->p_free_list     = sp_mem;
                    sp_mpl->p_use_list      = NULL;
                    sp_mpl->pool_mtx_id     = work_mtx_id;
                    result                  = index;
                }
                break;
            }
        }
    }

    return result;
}

#if(0)
/**************************************************************************//**
* Function Name: R_UTIL_DeletePool<br>
* @brief         Memory pool delete processing<br>
*
*                Description:<br>
*                Removal process Memory pool<br>
* @param[in]     pool_id         :Pool control number
* @retval        R_UTIL_RET_OK             :Success.
* @retval        R_UTIL_RET_ERROR_ID       :The parameter signal_id is incorrect.
* @retval        R_UTIL_RET_ERROR_ISR      :This API cannot be called from interrupt service routines.
* @retval        R_UTIL_RET_ERROR_INIT     :Initialization isn't complete.
******************************************************************************/
static int32_t R_UTIL_DeletePool(int32_t const pool_id)
{
    int32_t result;

    /* Checks Arguments */
    if ((pool_id < 0) || (pool_id >= MPL_TBL_MAX) || (sa_mpl[pool_id].pool_mtx_id == NULL))
    {
        result = R_UTIL_RET_ERROR_ID;
    }
    else
    {
        mpl_st * const sp_mpl = &sa_mpl[pool_id];

        /* Deletes a specified memory pool */
        (void)osMutexAcquire(sp_mpl->pool_mtx_id, UTIL_MTX_WAIT_TIME);
        (void)osMutexDelete(sp_mpl->pool_mtx_id);
        (void)memset(sp_mpl, 0, sizeof(mpl_st));
        result = R_UTIL_RET_OK;
    }

    return result;
}
#endif

/**************************************************************************//**
* Function Name: R_UTIL_GetPool<br>
* @brief         Memory get processing<br>
*
*                Description:<br>
*                Memory get processing<br>
* @param[in]     pool_id         :Pool control number
* @param[in]     get_size        :Signals
* @param[in]     p_get_mem       :Wait mode
* @retval        R_UTIL_RET_OK             :Success.
* @retval        R_UTIL_RET_ERROR_RESOURCE :Maximum resource count.
* @retval        R_UTIL_RET_ERROR_ID       :The parameter signal_id is incorrect.
* @retval        R_UTIL_RET_ERROR_ISR      :This API cannot be called from interrupt service routines.
* @retval        R_UTIL_RET_ERROR_TIMEOUT  :Signal not occurred within timeout.
* @retval        R_UTIL_RET_ERROR_INIT     :Initialization isn't complete.
* @retval        R_UTIL_RET_ERROR_PARAMETER:Parameter error.
******************************************************************************/
static int32_t R_UTIL_GetPool(int32_t const pool_id, uint32_t const get_size, void ** const pp_get_mem)
{
    int32_t        result;
    uint32_t const work_get_size = ((get_size + CHK_ALIGN_4) & ~CHK_ALIGN_4) + sizeof(mem_st);

    /* Checks Arguments */
    if ((pool_id < 0) || (pool_id >= MPL_TBL_MAX) || (sa_mpl[pool_id].pool_mtx_id == NULL))
    {
        result = R_UTIL_RET_ERROR_ID;
    }
    else if ((pp_get_mem == NULL) || (((uint32_t)pp_get_mem & CHK_ALIGN_4) != 0uL)
          || (work_get_size >= sa_mpl[pool_id].total_size))
    {
        result = R_UTIL_RET_ERROR_PARAMETER;
    }
    /* Waits until a Mutex becomes available */
    else if (osMutexAcquire(sa_mpl[pool_id].pool_mtx_id, UTIL_MTX_WAIT_TIME) != osOK)
    {
        result = R_UTIL_RET_ERROR_ISR;
    }
    else
    {
        mpl_st * const sp_mpl      = &sa_mpl[pool_id];
        mem_st *  sp_free_mem      = sp_mpl->p_free_list;
        mem_st *  sp_pre_mem       = NULL;
        mem_st *  sp_bestfit_mem   = NULL;
        mem_st ** spp_pre_mem_next = &sp_mpl->p_free_list;

        /* Searches a usable space */
        while (sp_free_mem != NULL)
        {
            if ((sp_free_mem->size >= work_get_size)
             && ((sp_bestfit_mem == NULL) || ((sp_bestfit_mem->size) > (sp_free_mem->size))))
            {
                sp_bestfit_mem = sp_free_mem;
                if (sp_pre_mem != NULL)
                {
                    spp_pre_mem_next = &sp_pre_mem->sp_next;
                }
                if (sp_free_mem->size == work_get_size)
                {
                    break;
                }
            }
            sp_pre_mem  = sp_free_mem;
            sp_free_mem = sp_free_mem->sp_next;
        }

        if (sp_bestfit_mem == NULL)
        {
            /* no usable space */
            *pp_get_mem = NULL;
            result      = R_UTIL_RET_ERROR_RESOURCE;
        }
        else
        {
            if ((sp_bestfit_mem->size - work_get_size) >= sizeof(mem_st))
            {
                /* Split the space, to use the first part. */
                mem_st * const sp_next_free_mem = (mem_st *)((uint8_t *)sp_bestfit_mem + work_get_size);

                sp_next_free_mem->size    = sp_bestfit_mem->size - work_get_size;
                sp_next_free_mem->sp_next = sp_bestfit_mem->sp_next;
                sp_bestfit_mem->size      = work_get_size;
                *spp_pre_mem_next         = sp_next_free_mem;
            }
            else
            {
                /* Removed from the free-list. */
                *spp_pre_mem_next = sp_bestfit_mem->sp_next;
            }

            /* Add to the top of the use-list. */
            sp_bestfit_mem->sp_next = sp_mpl->p_use_list;
            sp_mpl->p_use_list      = sp_bestfit_mem;

            /* Set the memory space. */
            *pp_get_mem = (void *)((uint8_t *)sp_bestfit_mem + sizeof(mem_st));
            result      = R_UTIL_RET_OK;
        }
        (void)osMutexRelease(sp_mpl->pool_mtx_id);
    }

    return result;
}

/**************************************************************************//**
* Function Name: R_UTIL_RelPool<br>
* @brief         Signal wait processing<br>
*
*                Description:<br>
*                Signal wait processing<br>
* @param[in]     signal_id       :Signal control number
* @param[in]     signals         :Signals
* @param[in]     mode            :Wait mode
* @param[in]     p_signal_ptn    :Signal pattern causing a task to be released form waiting
* @param[in]     timeout         :Value of timeout[system tick].
* @retval        R_UTIL_RET_OK             :Success.
* @retval        R_UTIL_RET_ERROR_RESOURCE :Maximum resource count.
* @retval        R_UTIL_RET_ERROR_ID       :The parameter signal_id is incorrect.
* @retval        R_UTIL_RET_ERROR_ISR      :This API cannot be called from interrupt service routines.
* @retval        R_UTIL_RET_ERROR_TIMEOUT  :Signal not occurred within timeout.
* @retval        R_UTIL_RET_ERROR_INIT     :Initialization isn't complete.
* @retval        R_UTIL_RET_ERROR_PARAMETER:Parameter error.
******************************************************************************/
static int32_t R_UTIL_RelPool(int32_t const pool_id, void * const p_rel_mem)
{
    int32_t result;

    /* Checks Arguments */
    if ((pool_id < 0) || (pool_id >= MPL_TBL_MAX) || (sa_mpl[pool_id].pool_mtx_id == NULL))
    {
        result = R_UTIL_RET_ERROR_ID;
    }
    else if ((p_rel_mem == NULL) || (((uint32_t)p_rel_mem & CHK_ALIGN_4) != 0uL) || ((uint32_t)p_rel_mem <= sizeof(mem_st)))
    {
        result = R_UTIL_RET_ERROR_PARAMETER;
    }
    else
    {
        mpl_st * const sp_mpl     = &sa_mpl[pool_id];
        mem_st * const sp_del_mem = (mem_st *)(uint8_t *)((uint32_t)p_rel_mem - sizeof(mem_st));

        /* Check the range of memory. */
        if ((sp_del_mem < sp_mpl->p_top_mem)
         || (((uint32_t)sp_del_mem + sp_del_mem->size) > ((uint32_t)sp_mpl->p_top_mem + sp_mpl->total_size)))
        {
            result = R_UTIL_RET_ERROR_PARAMETER;
        }
        /* Waits until a Mutex becomes available */
        else if (osMutexAcquire(sa_mpl[pool_id].pool_mtx_id, UTIL_MTX_WAIT_TIME) != osOK)
        {
            result = R_UTIL_RET_ERROR_ISR;
        }
        else
        {
            mem_st *  sp_mem;
            mem_st ** spp_pre_mem_next;
            mem_st *  sp_pre_mem;

            /* Check the memory has been registered in the use-list. */
            sp_mem            = sp_mpl->p_use_list;
            spp_pre_mem_next  = &sp_mpl->p_use_list;
            sp_pre_mem        = NULL;
            while (sp_mem != NULL)
            {
                if (sp_mem == sp_del_mem)
                {
                    /* Removed from the use-list. */
                    if (sp_pre_mem != NULL)
                    {
                        spp_pre_mem_next = &sp_pre_mem->sp_next;
                    }
                    *spp_pre_mem_next  = sp_mem->sp_next;
                    break;
                }
                sp_pre_mem = sp_mem;
                sp_mem     = sp_mem->sp_next;
            }
            if (sp_mem == NULL)
            {
                /* It ends, when not registering with a use-list. */
                result = R_UTIL_RET_ERROR_RESOURCE;
            }
            else
            {
                uint32_t  chek_flg        = 0uL;
                mem_st *  sp_pre_address  = NULL;
                mem_st *  sp_next_address = NULL;

                /* It is searched whether there is any adjoining memory */
                sp_mem            =  sp_mpl->p_free_list;
                spp_pre_mem_next  =  &sp_mpl->p_free_list;
                sp_pre_mem        =  NULL;
                while ((sp_mem != NULL) && (chek_flg != (ADJOINING_PREV | ADJOINING_NEXT)))
                {
                    if (((uint32_t)sp_mem + sp_mem->size) == (uint32_t)sp_del_mem)
                    {
                        sp_pre_address  =  sp_mem;
                        chek_flg        |= ADJOINING_PREV;
                    }
                    else if (((uint32_t)sp_del_mem + sp_del_mem->size) == (uint32_t)sp_mem)
                    {
                        sp_next_address =  sp_mem;
                        if (sp_pre_mem != NULL)
                        {
                            spp_pre_mem_next = &sp_pre_mem->sp_next;
                        }
                        chek_flg        |= ADJOINING_NEXT;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                    sp_pre_mem = sp_mem;
                    sp_mem     = sp_mem->sp_next;
                }

                if ((sp_pre_address != NULL) && (sp_next_address != NULL))
                {
                    /* It joins to the memory of previous and next. */
                    *spp_pre_mem_next    =  sp_next_address->sp_next;
                    sp_pre_address->size += (sp_del_mem->size + sp_next_address->size);
                }
                else if (sp_pre_address != NULL)
                {
                    /* It joints to the previous memory. */
                    sp_pre_address->size += sp_del_mem->size;
                }
                else if (sp_next_address != NULL)
                {
                    /* It joints to the next memory. */
                    *spp_pre_mem_next   =  sp_del_mem;
                    sp_del_mem->sp_next =  sp_next_address->sp_next;
                    sp_del_mem->size    += sp_next_address->size;
                }
                else
                {
                    /* It adds to the head of a free-list. */
                    sp_del_mem->sp_next = sp_mpl->p_free_list;
                    sp_mpl->p_free_list = sp_del_mem;
                }
                (void)osMutexRelease(sp_mpl->pool_mtx_id);
                result = R_UTIL_RET_OK;
            }
        }
    }

    return result;
}

