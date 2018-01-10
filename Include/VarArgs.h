/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

/**
 * @file VarArgs.h
 */

/**
 * THESE MACROS
 */
#define VARARG_EXTRA(A) A,
#define VARARG_NONE
#define VARARG_PURE =0

static inline int			CheckVA(int i)              { return i; }
static inline int		CheckVA(uint32_t ui)        { return (int)ui; }
//static inline uint64		CheckVA(uint64 qw)          { return qw; }
//static inline int64		CheckVA(int64 sqw)          { return sqw; }
static inline double		CheckVA(double d)           { return d; }
static inline long		    CheckVA(long d)             { return d; }
static inline long		    CheckVA(unsigned long d)    { return d; }
static inline char			CheckVA(char c)             { return c; }
//static inline void*			CheckVA(char* s)            { return (void*)s; }
static inline char*			CheckVA(char* s)            { return s; }
static inline bool		    CheckVA(bool b)             { return b; }
template<class T> T*		CheckVA(T* p)               { return p; }
template<class T> const T*	CheckVA(const T* p)         { return p; }


#define VARARG_DECL( FuncRet, StaticFuncRet, Return, FuncName, Pure, FormatType, ExtraDecl, ExtraCall )	\
	FuncRet FuncName##__VA( ExtraDecl FormatType Format, ... ) Pure;  \
	StaticFuncRet FuncName(ExtraDecl FormatType Format) {Return FuncName##__VA(ExtraCall (Format));} \
	template<class T1> \
	StaticFuncRet FuncName(ExtraDecl FormatType Format,T1 V1) {T1 v1=CheckVA(V1);Return FuncName##__VA(ExtraCall (Format),(v1));} \
	template<class T1,class T2> \
	StaticFuncRet FuncName(ExtraDecl FormatType Format,T1 V1,T2 V2) {T1 v1=CheckVA(V1);T2 v2=CheckVA(V2);Return FuncName##__VA(ExtraCall (Format),(v1),(v2));} \
	template<class T1,class T2,class T3> \
	StaticFuncRet FuncName(ExtraDecl FormatType Format,T1 V1,T2 V2,T3 V3) {T1 v1=CheckVA(V1);T2 v2=CheckVA(V2);T3 v3=CheckVA(V3);Return FuncName##__VA(ExtraCall (Format),(v1),(v2),(v3));} \
	template<class T1,class T2,class T3,class T4> \
	StaticFuncRet FuncName(ExtraDecl FormatType Format,T1 V1,T2 V2,T3 V3,T4 V4) {T1 v1=CheckVA(V1);T2 v2=CheckVA(V2);T3 v3=CheckVA(V3);T4 v4=CheckVA(V4);Return FuncName##__VA(ExtraCall (Format),(v1),(v2),(v3),(v4));} \
	template<class T1,class T2,class T3,class T4,class T5> \
	StaticFuncRet FuncName(ExtraDecl FormatType Format,T1 V1,T2 V2,T3 V3,T4 V4,T5 V5) {T1 v1=CheckVA(V1);T2 v2=CheckVA(V2);T3 v3=CheckVA(V3);T4 v4=CheckVA(V4);T5 v5=CheckVA(V5);Return FuncName##__VA(ExtraCall (Format),(v1),(v2),(v3),(v4),(v5));}

#define VARARG_BODY( FuncRet, FuncName, FormatType, ExtraDecl )		\
		FuncRet FuncName##__VA( ExtraDecl  FormatType Format, ... )

