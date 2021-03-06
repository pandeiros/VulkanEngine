/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#pragma once

/**
 * @file VarArgs.h
 */

#define VARARG_EXTRA(A) A,
#define VARARG_NONE
#define VARARG_PURE =0

static inline int			CheckVA(int i)              { return i; }
static inline uint32_t		CheckVA(uint32_t ui)        { return ui; }
static inline float		    CheckVA(float f)            { return f; }
static inline double		CheckVA(double d)           { return d; }
static inline long		    CheckVA(long l)             { return l; }
static inline long		    CheckVA(unsigned long ul)   { return ul; }
static inline char			CheckVA(char c)             { return c; }
static inline const char*   CheckVA(std::string s)      { return s.c_str(); }
static inline char*			CheckVA(char* s)            { return s; }
static inline bool		    CheckVA(bool b)             { return b; }
template<class T> T*		CheckVA(T* p)               { return p; }
template<class T> const T*	CheckVA(const T* p)         { return p; }

#ifndef __ANDROID__
static inline size_t        CheckVA(size_t u)           { return u; }
#endif

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

