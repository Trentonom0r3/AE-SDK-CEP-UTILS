#pragma once

/*
Create definitions to auto-export the functions in this DLL.
This works well for Windows, the Mac and HP/UX. For Linux
and Solaris, export everything, and for AIX, create an export
definition file. You will need to add the constants for Unix
operating systems to your makefile.
����DLL���̊֐��������I�ɃG�N�X�|�[�g���邽�߂̒�`���쐬���܂��B
�����Windows�AMac�AHP/UX�ł͂��܂����삵�܂��B
Linux��Solaris�ł͂��ׂĂ��G�N�X�|�[�g���AAIX�ł̓G�N�X�|�[�g��`�t�@�C�����쐬���܂��B
Unix�I�y���[�e�B���O�V�X�e���̒萔��makefile�ɒǉ�����K�v������܂��B
*/


#if defined (_WINDOWS) || defined (HPUX_ACC)
#define EXPORT __declspec(dllexport)
#elif defined(__APPLE__)
#define EXPORT __attribute__((visibility("default")))
#elif defined (LINUX) || defined (SOLARIS) || defined (AIX_VACPP6)
#define DLL_EXPORT
/* fine, just create the exports file for AIX
   you will have to link your app with dl.so
   so you can resolve dlopen, dlclose and dlsym.
*/
#else
#error Unsupported compiler
#endif