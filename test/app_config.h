//
// File: app_config.h
// Author: Darren Erik Vengroff <darrenv@eecs.umich.edu>
// Created: 10/6/94
//
// $Id: app_config.h,v 1.22 2002-01-15 03:53:03 tavi Exp $
//
#ifndef _APP_CONFIG_H
#define _APP_CONFIG_H

// Get the configuration as set up by the TPIE configure script.
#include <config.h>

/* ********************************************************************** */
/*                      developer use                                     */
/* ********************************************************************** */


/* ********************************************************************** */
/*                  choose default BTE collection implementation          */
/* ********************************************************************** */

/* Pick a version of BTE collections; default is BTE_COLLECTION_IMP_MMAP */
#define BTE_COLLECTION_IMP_MMAP
//#define BTE_COLLECTION_IMP_UFS

/* ********************************************************************** */
/*                       choose BTE stream implementation                 */
/* ********************************************************************** */

/* Pick a version of BTE streams; default is BTE_STREAM_IMP_UFS */
//#define BTE_STREAM_IMP_MMAP
//#define BTE_STREAM_IMP_STDIO
#define BTE_STREAM_IMP_UFS


/* ********************************************************************** */
/*                      configure BTE                                     */
/* ********************************************************************** */


/* ********************************************************************** */
/*                      BTE_MMB configuration options                     */
/* ********************************************************************** */
#ifdef BTE_STREAM_IMP_MMAP

/* define logical blocksize; default is 32 * operating system blocksize */
#define BTE_STREAM_MMAP_BLOCK_FACTOR 32

/* enable/disable TPIE read ahead; default is enabled (set to 1) */
#define BTE_MMB_READ_AHEAD 1

/* read ahead method, ignored unless BTE_MMB_READ_AHEAD is set to 1;
   if USE_LIBAIO is enabled, use asynchronous IO read ahead; otherwise
   use use mmap-based read ahead; default is mmap-based read ahead
   (USE_LIBAIO not defined) */
//#define USE_LIBAIO

#endif


/* ********************************************************************** */
/*                      BTE_UFS configuration options                     */
/* ********************************************************************** */
#ifdef BTE_STREAM_IMP_UFS

/* define logical blocksize; default is 32 * operating system blocksize */
#define BTE_STREAM_UFS_BLOCK_FACTOR 32

/* enable/disable TPIE read ahead; default is disabled (set to 0) */
#define BTE_UFS_READ_AHEAD 0

/* read ahead method, ignored unless BTE_UFS_READ_AHEAD is set to 1;
   if USE_LIBAIO is set to 1, use asynchronous IO read ahead;
   otherwise no TPIE read ahead is done; default is disabled (set to
   0) */
#define USE_LIBAIO 0

#endif
/********************************************************************/


/********************************************************************/
/*              Set up some defaults for the apps                   */
/********************************************************************/
#include <sys/types.h> // for size_t
#include <stdlib.h> // for random

#define DEFAULT_TEST_SIZE (20000000)
#define DEFAULT_TEST_MM_SIZE (1024 * 1024 * 32)

extern bool verbose;
extern size_t test_mm_size;
extern size_t test_size;
extern int random_seed;
/********************************************************************/


/********************************************************************/
/*                            logging;                              */
/*              this should NOT be modified by user!!!              */
/*       in order to enable/disable library/application logging,    */
/*     run tpie configure script with appropriate options           */
/********************************************************************/
// Use logs if requested.
#if TP_LOG_APPS
#define TPL_LOGGING 1
#endif

#include <tpie_log.h>

// Enable assertions if requested.
#if TP_ASSERT_APPS
#define DEBUG_ASSERTIONS 1
#endif
#include <tpie_assert.h>
/********************************************************************/

#endif
