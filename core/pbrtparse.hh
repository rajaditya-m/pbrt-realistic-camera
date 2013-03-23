/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     STRING = 258,
     ID = 259,
     NUM = 260,
     LBRACK = 261,
     RBRACK = 262,
     ACCELERATOR = 263,
     ACTIVETRANSFORM = 264,
     ALL = 265,
     AREALIGHTSOURCE = 266,
     ATTRIBUTEBEGIN = 267,
     ATTRIBUTEEND = 268,
     CAMERA = 269,
     CONCATTRANSFORM = 270,
     COORDINATESYSTEM = 271,
     COORDSYSTRANSFORM = 272,
     ENDTIME = 273,
     FILM = 274,
     IDENTITY = 275,
     INCLUDE = 276,
     LIGHTSOURCE = 277,
     LOOKAT = 278,
     MAKENAMEDMATERIAL = 279,
     MATERIAL = 280,
     NAMEDMATERIAL = 281,
     OBJECTBEGIN = 282,
     OBJECTEND = 283,
     OBJECTINSTANCE = 284,
     PIXELFILTER = 285,
     RENDERER = 286,
     REVERSEORIENTATION = 287,
     ROTATE = 288,
     SAMPLER = 289,
     SCALE = 290,
     SHAPE = 291,
     STARTTIME = 292,
     SURFACEINTEGRATOR = 293,
     TEXTURE = 294,
     TRANSFORMBEGIN = 295,
     TRANSFORMEND = 296,
     TRANSFORMTIMES = 297,
     TRANSFORM = 298,
     TRANSLATE = 299,
     VOLUME = 300,
     VOLUMEINTEGRATOR = 301,
     WORLDBEGIN = 302,
     WORLDEND = 303,
     HIGH_PRECEDENCE = 304
   };
#endif
/* Tokens.  */
#define STRING 258
#define ID 259
#define NUM 260
#define LBRACK 261
#define RBRACK 262
#define ACCELERATOR 263
#define ACTIVETRANSFORM 264
#define ALL 265
#define AREALIGHTSOURCE 266
#define ATTRIBUTEBEGIN 267
#define ATTRIBUTEEND 268
#define CAMERA 269
#define CONCATTRANSFORM 270
#define COORDINATESYSTEM 271
#define COORDSYSTRANSFORM 272
#define ENDTIME 273
#define FILM 274
#define IDENTITY 275
#define INCLUDE 276
#define LIGHTSOURCE 277
#define LOOKAT 278
#define MAKENAMEDMATERIAL 279
#define MATERIAL 280
#define NAMEDMATERIAL 281
#define OBJECTBEGIN 282
#define OBJECTEND 283
#define OBJECTINSTANCE 284
#define PIXELFILTER 285
#define RENDERER 286
#define REVERSEORIENTATION 287
#define ROTATE 288
#define SAMPLER 289
#define SCALE 290
#define SHAPE 291
#define STARTTIME 292
#define SURFACEINTEGRATOR 293
#define TEXTURE 294
#define TRANSFORMBEGIN 295
#define TRANSFORMEND 296
#define TRANSFORMTIMES 297
#define TRANSFORM 298
#define TRANSLATE 299
#define VOLUME 300
#define VOLUMEINTEGRATOR 301
#define WORLDBEGIN 302
#define WORLDEND 303
#define HIGH_PRECEDENCE 304




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 157 "core/pbrtparse.yy"
{
char string[1024];
float num;
ParamArray *ribarray;
}
/* Line 1529 of yacc.c.  */
#line 153 "core/pbrtparse.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

