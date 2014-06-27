
main.o:     file format pe-i386


Disassembly of section .text:

00000000 <__Z5test1x>:
#include <random>
#include <cstdio>



long long test1(long long a){
   0:	55                   	push   ebp
   1:	89 e5                	mov    ebp,esp
   3:	53                   	push   ebx
   4:	83 ec 0c             	sub    esp,0xc
   7:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
   a:	89 45 f0             	mov    DWORD PTR [ebp-0x10],eax
   d:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
  10:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax
    return a * 9;
  13:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
  16:	8b 5d f4             	mov    ebx,DWORD PTR [ebp-0xc]
  19:	89 c8                	mov    eax,ecx
  1b:	89 da                	mov    edx,ebx
  1d:	0f a4 c2 03          	shld   edx,eax,0x3
  21:	c1 e0 03             	shl    eax,0x3
  24:	01 c8                	add    eax,ecx
  26:	11 da                	adc    edx,ebx
}
  28:	83 c4 0c             	add    esp,0xc
  2b:	5b                   	pop    ebx
  2c:	5d                   	pop    ebp
  2d:	c3                   	ret    

0000002e <_main>:
    return internal.value*9;
    }
}BindFast;
#endif
using namespace GlobalMUD;
int main(){
  2e:	8d 4c 24 04          	lea    ecx,[esp+0x4]
  32:	83 e4 f0             	and    esp,0xfffffff0
  35:	ff 71 fc             	push   DWORD PTR [ecx-0x4]
  38:	55                   	push   ebp
  39:	89 e5                	mov    ebp,esp
  3b:	57                   	push   edi
  3c:	56                   	push   esi
  3d:	53                   	push   ebx
  3e:	51                   	push   ecx
  3f:	83 ec 78             	sub    esp,0x78
  42:	c7 45 ac 00 00 00 00 	mov    DWORD PTR [ebp-0x54],0x0
  49:	c7 45 b0 00 00 00 00 	mov    DWORD PTR [ebp-0x50],0x0
  50:	8d 45 b4             	lea    eax,[ebp-0x4c]
  53:	8d 55 e8             	lea    edx,[ebp-0x18]
  56:	89 10                	mov    DWORD PTR [eax],edx
  58:	ba fb 00 00 00       	mov    edx,0xfb
  5d:	89 50 04             	mov    DWORD PTR [eax+0x4],edx
  60:	89 60 08             	mov    DWORD PTR [eax+0x8],esp
  63:	8d 45 94             	lea    eax,[ebp-0x6c]
  66:	89 04 24             	mov    DWORD PTR [esp],eax
  69:	e8 00 00 00 00       	call   6e <_main+0x40>
  6e:	e8 00 00 00 00       	call   73 <_main+0x45>

    //printf("%s\n\n",ERROR::ToString(Error::NotAnError).c_str());
    Callback<long long>cb1(test1,9);
  73:	8d 45 d4             	lea    eax,[ebp-0x2c]
  76:	c7 44 24 04 09 00 00 	mov    DWORD PTR [esp+0x4],0x9
  7d:	00 
  7e:	c7 04 24 00 00 00 00 	mov    DWORD PTR [esp],0x0
  85:	c7 45 98 01 00 00 00 	mov    DWORD PTR [ebp-0x68],0x1
  8c:	89 c1                	mov    ecx,eax
  8e:	e8 00 00 00 00       	call   93 <_main+0x65>
  93:	83 ec 08             	sub    esp,0x8

for( unsigned long long i = 0; i < 100000ll*10000ll; ++i ){
        #endif // POOP


    t = test1(5);
  96:	c7 04 24 05 00 00 00 	mov    DWORD PTR [esp],0x5
  9d:	c7 44 24 04 00 00 00 	mov    DWORD PTR [esp+0x4],0x0
  a4:	00 
  a5:	e8 56 ff ff ff       	call   0 <__Z5test1x>
  aa:	89 45 e0             	mov    DWORD PTR [ebp-0x20],eax
  ad:	89 55 e4             	mov    DWORD PTR [ebp-0x1c],edx
printf("\n\nTime taken for standard function call: %d seconds\n", time(0) - a);
a = time(0);
for( unsigned long long i = 0; i < 10000ll*10000ll; ++i ){
        #endif // POOP

    t = cb1.Call();
  b0:	8d 45 d4             	lea    eax,[ebp-0x2c]
  b3:	c7 45 98 02 00 00 00 	mov    DWORD PTR [ebp-0x68],0x2
  ba:	89 c1                	mov    ecx,eax
  bc:	e8 00 00 00 00       	call   c1 <_main+0x93>
  c1:	89 45 e0             	mov    DWORD PTR [ebp-0x20],eax
  c4:	89 55 e4             	mov    DWORD PTR [ebp-0x1c],edx
  c7:	8d 45 d4             	lea    eax,[ebp-0x2c]
  ca:	c7 45 98 01 00 00 00 	mov    DWORD PTR [ebp-0x68],0x1
  d1:	89 c1                	mov    ecx,eax
  d3:	e8 00 00 00 00       	call   d8 <_main+0xaa>
    Test<GlobalMUD::Ciphers::XOR>();
    Test<GlobalMUD::CommStream>();
    Test<GlobalMUD::HTTPd>();
    #endif // POOP

}
  d8:	b8 00 00 00 00       	mov    eax,0x0
  dd:	89 45 90             	mov    DWORD PTR [ebp-0x70],eax
  e0:	eb 3e                	jmp    120 <_main+0xf2>
  e2:	89 55 8c             	mov    DWORD PTR [ebp-0x74],edx
printf("\n\nTime taken for standard function call: %d seconds\n", time(0) - a);
a = time(0);
for( unsigned long long i = 0; i < 10000ll*10000ll; ++i ){
        #endif // POOP

    t = cb1.Call();
  e5:	8d 45 d4             	lea    eax,[ebp-0x2c]
  e8:	c7 45 98 00 00 00 00 	mov    DWORD PTR [ebp-0x68],0x0
  ef:	89 c1                	mov    ecx,eax
  f1:	e8 00 00 00 00       	call   f6 <_main+0xc8>
  f6:	8b 45 8c             	mov    eax,DWORD PTR [ebp-0x74]
  f9:	eb 16                	jmp    111 <_main+0xe3>
  fb:	8d 6d 18             	lea    ebp,[ebp+0x18]
  fe:	8b 55 9c             	mov    edx,DWORD PTR [ebp-0x64]
 101:	8b 45 98             	mov    eax,DWORD PTR [ebp-0x68]
 104:	85 c0                	test   eax,eax
 106:	74 07                	je     10f <_main+0xe1>
 108:	83 f8 01             	cmp    eax,0x1
 10b:	74 d5                	je     e2 <_main+0xb4>
 10d:	0f 0b                	ud2    
 10f:	89 d0                	mov    eax,edx
 111:	89 04 24             	mov    DWORD PTR [esp],eax
 114:	c7 45 98 ff ff ff ff 	mov    DWORD PTR [ebp-0x68],0xffffffff
 11b:	e8 00 00 00 00       	call   120 <_main+0xf2>
 120:	8d 45 94             	lea    eax,[ebp-0x6c]
 123:	89 04 24             	mov    DWORD PTR [esp],eax
 126:	e8 00 00 00 00       	call   12b <_main+0xfd>
    Test<GlobalMUD::Ciphers::XOR>();
    Test<GlobalMUD::CommStream>();
    Test<GlobalMUD::HTTPd>();
    #endif // POOP

}
 12b:	8b 45 90             	mov    eax,DWORD PTR [ebp-0x70]
 12e:	8d 65 f0             	lea    esp,[ebp-0x10]
 131:	59                   	pop    ecx
 132:	5b                   	pop    ebx
 133:	5e                   	pop    esi
 134:	5f                   	pop    edi
 135:	5d                   	pop    ebp
 136:	8d 61 fc             	lea    esp,[ecx-0x4]
 139:	c3                   	ret    
 13a:	90                   	nop
 13b:	90                   	nop

Disassembly of section .text$_ZN9GlobalMUD8Private215StorageInternalIIEEC1Ev:

00000000 <__ZN9GlobalMUD8Private215StorageInternalIIEEC1Ev>:
            }
        };
        template<>
        struct StorageInternal<>{
            static constexpr int type = 3;
            StorageInternal(){
   0:	55                   	push   ebp
   1:	89 e5                	mov    ebp,esp
   3:	83 ec 04             	sub    esp,0x4
   6:	89 4d fc             	mov    DWORD PTR [ebp-0x4],ecx
            }
   9:	c9                   	leave  
   a:	c3                   	ret    
   b:	90                   	nop

Disassembly of section .text$_ZN9GlobalMUD8CallbackIxIEEC1IPFxxEIiEEET_DpT0_:

00000000 <__ZN9GlobalMUD8CallbackIxIEEC1IPFxxEIiEEET_DpT0_>:
            Private2::Storage<Function, Arguments...> *store = (Private2::Storage<Function, Arguments...>*)d;
        }*/

    public:
        template<class Function, class... Argument>
        Callback(Function f, Argument... arg){
   0:	55                   	push   ebp
   1:	89 e5                	mov    ebp,esp
   3:	57                   	push   edi
   4:	56                   	push   esi
   5:	53                   	push   ebx
   6:	83 ec 6c             	sub    esp,0x6c
   9:	89 4d d4             	mov    DWORD PTR [ebp-0x2c],ecx
   c:	c7 45 b8 00 00 00 00 	mov    DWORD PTR [ebp-0x48],0x0
  13:	c7 45 bc 08 00 00 00 	mov    DWORD PTR [ebp-0x44],0x8
  1a:	8d 45 c0             	lea    eax,[ebp-0x40]
  1d:	8d 55 e8             	lea    edx,[ebp-0x18]
  20:	89 10                	mov    DWORD PTR [eax],edx
  22:	ba 91 00 00 00       	mov    edx,0x91
  27:	89 50 04             	mov    DWORD PTR [eax+0x4],edx
  2a:	89 60 08             	mov    DWORD PTR [eax+0x8],esp
  2d:	8d 45 a0             	lea    eax,[ebp-0x60]
  30:	89 04 24             	mov    DWORD PTR [esp],eax
  33:	e8 00 00 00 00       	call   38 <__ZN9GlobalMUD8CallbackIxIEEC1IPFxxEIiEEET_DpT0_+0x38>
            Private2::Storage<Function, Argument...> *store = new Private2::Storage<Function, Argument...>(f, arg...);
  38:	c7 04 24 0c 00 00 00 	mov    DWORD PTR [esp],0xc
  3f:	c7 45 a4 ff ff ff ff 	mov    DWORD PTR [ebp-0x5c],0xffffffff
  46:	e8 00 00 00 00       	call   4b <__ZN9GlobalMUD8CallbackIxIEEC1IPFxxEIiEEET_DpT0_+0x4b>
  4b:	89 45 9c             	mov    DWORD PTR [ebp-0x64],eax
  4e:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
  51:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
  55:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  58:	89 04 24             	mov    DWORD PTR [esp],eax
  5b:	c7 45 a4 01 00 00 00 	mov    DWORD PTR [ebp-0x5c],0x1
  62:	8b 4d 9c             	mov    ecx,DWORD PTR [ebp-0x64]
  65:	e8 00 00 00 00       	call   6a <__ZN9GlobalMUD8CallbackIxIEEC1IPFxxEIiEEET_DpT0_+0x6a>
  6a:	83 ec 08             	sub    esp,0x8
  6d:	8b 45 9c             	mov    eax,DWORD PTR [ebp-0x64]
  70:	89 45 e4             	mov    DWORD PTR [ebp-0x1c],eax
            storage = (void*) store;
  73:	8b 45 d4             	mov    eax,DWORD PTR [ebp-0x2c]
  76:	8b 55 e4             	mov    edx,DWORD PTR [ebp-0x1c]
  79:	89 10                	mov    DWORD PTR [eax],edx
            FreeStorage = Private2::Storage<Function, Argument...>::DeleteFromVoid;
  7b:	8b 45 d4             	mov    eax,DWORD PTR [ebp-0x2c]
  7e:	c7 40 08 00 00 00 00 	mov    DWORD PTR [eax+0x8],0x0
            cbptr = CallMe<Function, Argument...>;
  85:	8b 45 d4             	mov    eax,DWORD PTR [ebp-0x2c]
  88:	c7 40 04 00 00 00 00 	mov    DWORD PTR [eax+0x4],0x0
  8f:	eb 26                	jmp    b7 <__ZN9GlobalMUD8CallbackIxIEEC1IPFxxEIiEEET_DpT0_+0xb7>
  91:	8d 6d 18             	lea    ebp,[ebp+0x18]
  94:	8b 45 a8             	mov    eax,DWORD PTR [ebp-0x58]
  97:	89 45 98             	mov    DWORD PTR [ebp-0x68],eax
        }*/

    public:
        template<class Function, class... Argument>
        Callback(Function f, Argument... arg){
            Private2::Storage<Function, Argument...> *store = new Private2::Storage<Function, Argument...>(f, arg...);
  9a:	8b 55 9c             	mov    edx,DWORD PTR [ebp-0x64]
  9d:	89 14 24             	mov    DWORD PTR [esp],edx
  a0:	e8 00 00 00 00       	call   a5 <__ZN9GlobalMUD8CallbackIxIEEC1IPFxxEIiEEET_DpT0_+0xa5>
  a5:	8b 45 98             	mov    eax,DWORD PTR [ebp-0x68]
  a8:	89 04 24             	mov    DWORD PTR [esp],eax
  ab:	c7 45 a4 ff ff ff ff 	mov    DWORD PTR [ebp-0x5c],0xffffffff
  b2:	e8 00 00 00 00       	call   b7 <__ZN9GlobalMUD8CallbackIxIEEC1IPFxxEIiEEET_DpT0_+0xb7>
  b7:	8d 45 a0             	lea    eax,[ebp-0x60]
  ba:	89 04 24             	mov    DWORD PTR [esp],eax
  bd:	e8 00 00 00 00       	call   c2 <__ZN9GlobalMUD8CallbackIxIEEC1IPFxxEIiEEET_DpT0_+0xc2>
            storage = (void*) store;
            FreeStorage = Private2::Storage<Function, Argument...>::DeleteFromVoid;
            cbptr = CallMe<Function, Argument...>;
        }
  c2:	8d 65 f4             	lea    esp,[ebp-0xc]
  c5:	5b                   	pop    ebx
  c6:	5e                   	pop    esi
  c7:	5f                   	pop    edi
  c8:	5d                   	pop    ebp
  c9:	c2 08 00             	ret    0x8

Disassembly of section .text$_ZN9GlobalMUD8CallbackIxIEED1Ev:

00000000 <__ZN9GlobalMUD8CallbackIxIEED1Ev>:
        ReturnType Call(RequiredArgs... args){
            return cbptr(storage, args...);
        }
        ~Callback(){
   0:	55                   	push   ebp
   1:	89 e5                	mov    ebp,esp
   3:	83 ec 28             	sub    esp,0x28
   6:	89 4d f4             	mov    DWORD PTR [ebp-0xc],ecx
            FreeStorage(storage);
   9:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
   c:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
   f:	8b 55 f4             	mov    edx,DWORD PTR [ebp-0xc]
  12:	8b 12                	mov    edx,DWORD PTR [edx]
  14:	89 14 24             	mov    DWORD PTR [esp],edx
  17:	ff d0                	call   eax
        }
  19:	c9                   	leave  
  1a:	c3                   	ret    
  1b:	90                   	nop

Disassembly of section .text$_ZN9GlobalMUD8CallbackIxIEE4CallEv:

00000000 <__ZN9GlobalMUD8CallbackIxIEE4CallEv>:
            Private2::Storage<Function, Argument...> *store = new Private2::Storage<Function, Argument...>(f, arg...);
            storage = (void*) store;
            FreeStorage = Private2::Storage<Function, Argument...>::DeleteFromVoid;
            cbptr = CallMe<Function, Argument...>;
        }
        ReturnType Call(RequiredArgs... args){
   0:	55                   	push   ebp
   1:	89 e5                	mov    ebp,esp
   3:	83 ec 28             	sub    esp,0x28
   6:	89 4d f4             	mov    DWORD PTR [ebp-0xc],ecx
            return cbptr(storage, args...);
   9:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
   c:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
   f:	8b 55 f4             	mov    edx,DWORD PTR [ebp-0xc]
  12:	8b 12                	mov    edx,DWORD PTR [edx]
  14:	89 14 24             	mov    DWORD PTR [esp],edx
  17:	ff d0                	call   eax
        }
  19:	c9                   	leave  
  1a:	c3                   	ret    
  1b:	90                   	nop

Disassembly of section .text$_ZN9GlobalMUD8Private27StorageIPFxxEIiEEC1ES3_i:

00000000 <__ZN9GlobalMUD8Private27StorageIPFxxEIiEEC1ES3_i>:
        };
        template<class Function, class... arguments>
        struct Storage{
            Function f;
            StorageInternal<arguments...> vals;
            Storage(Function ff, arguments... args) : vals(args...){
   0:	55                   	push   ebp
   1:	89 e5                	mov    ebp,esp
   3:	83 ec 28             	sub    esp,0x28
   6:	89 4d f4             	mov    DWORD PTR [ebp-0xc],ecx
   9:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
   c:	8d 50 04             	lea    edx,[eax+0x4]
   f:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
  12:	89 04 24             	mov    DWORD PTR [esp],eax
  15:	89 d1                	mov    ecx,edx
  17:	e8 00 00 00 00       	call   1c <__ZN9GlobalMUD8Private27StorageIPFxxEIiEEC1ES3_i+0x1c>
  1c:	83 ec 04             	sub    esp,0x4
                f = ff;
  1f:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
  22:	8b 55 08             	mov    edx,DWORD PTR [ebp+0x8]
  25:	89 10                	mov    DWORD PTR [eax],edx
            }
  27:	c9                   	leave  
  28:	c2 08 00             	ret    0x8
  2b:	90                   	nop

Disassembly of section .text$_ZN9GlobalMUD8Private27StorageIPFxxEIiEE14DeleteFromVoidEPv:

00000000 <__ZN9GlobalMUD8Private27StorageIPFxxEIiEE14DeleteFromVoidEPv>:
            static void DeleteFromVoid(void* ptr){
   0:	55                   	push   ebp
   1:	89 e5                	mov    ebp,esp
   3:	83 ec 28             	sub    esp,0x28
                Storage<Function, arguments...> *store = (Storage<Function, arguments...>*) ptr;
   6:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
   9:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax
                delete store;
   c:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
   f:	89 04 24             	mov    DWORD PTR [esp],eax
  12:	e8 00 00 00 00       	call   17 <__ZN9GlobalMUD8Private27StorageIPFxxEIiEE14DeleteFromVoidEPv+0x17>
            }
  17:	c9                   	leave  
  18:	c3                   	ret    
  19:	90                   	nop
  1a:	90                   	nop
  1b:	90                   	nop

Disassembly of section .text$_ZN9GlobalMUD8CallbackIxIEE6CallMeIPFxxEIiEEExPv:

00000000 <__ZN9GlobalMUD8CallbackIxIEE6CallMeIPFxxEIiEEExPv>:
                return ((obj)->*(func))(args...);
            }
        };

        template<class Function, class... Arguments>
        static ReturnType CallMe(void*d, RequiredArgs... args){
   0:	55                   	push   ebp
   1:	89 e5                	mov    ebp,esp
   3:	57                   	push   edi
   4:	56                   	push   esi
   5:	53                   	push   ebx
   6:	83 ec 6c             	sub    esp,0x6c
   9:	c7 45 bc 00 00 00 00 	mov    DWORD PTR [ebp-0x44],0x0
  10:	c7 45 c0 0e 00 00 00 	mov    DWORD PTR [ebp-0x40],0xe
  17:	8d 45 c4             	lea    eax,[ebp-0x3c]
  1a:	8d 55 e8             	lea    edx,[ebp-0x18]
  1d:	89 10                	mov    DWORD PTR [eax],edx
  1f:	ba 75 00 00 00       	mov    edx,0x75
  24:	89 50 04             	mov    DWORD PTR [eax+0x4],edx
  27:	89 60 08             	mov    DWORD PTR [eax+0x8],esp
  2a:	8d 45 a4             	lea    eax,[ebp-0x5c]
  2d:	89 04 24             	mov    DWORD PTR [esp],eax
  30:	e8 00 00 00 00       	call   35 <__ZN9GlobalMUD8CallbackIxIEE6CallMeIPFxxEIiEEExPv+0x35>
            Private2::Storage<Function, Arguments...> *store = (Private2::Storage<Function, Arguments...>*)d;;
  35:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  38:	89 45 e4             	mov    DWORD PTR [ebp-0x1c],eax
            Private2::CallFunction<Function, ReturnType, Arguments...>a(*store);
  3b:	8d 4d d8             	lea    ecx,[ebp-0x28]
  3e:	89 4d a0             	mov    DWORD PTR [ebp-0x60],ecx
  41:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
  44:	89 e2                	mov    edx,esp
  46:	89 c3                	mov    ebx,eax
  48:	b8 03 00 00 00       	mov    eax,0x3
  4d:	89 d7                	mov    edi,edx
  4f:	89 de                	mov    esi,ebx
  51:	89 c1                	mov    ecx,eax
  53:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  55:	c7 45 a8 01 00 00 00 	mov    DWORD PTR [ebp-0x58],0x1
  5c:	8b 4d a0             	mov    ecx,DWORD PTR [ebp-0x60]
  5f:	e8 00 00 00 00       	call   64 <__ZN9GlobalMUD8CallbackIxIEE6CallMeIPFxxEIiEEExPv+0x64>
  64:	83 ec 0c             	sub    esp,0xc
            //Private2::Storage<Function, RequiredArgs...> store2(store->f, args...);*/
            //Private2::CallFunction<Function, ReturnType, Arguments...>a(*store, *store);


            //delete store2;
            return a.RetVal;
  67:	8b 45 d8             	mov    eax,DWORD PTR [ebp-0x28]
  6a:	8b 55 dc             	mov    edx,DWORD PTR [ebp-0x24]
  6d:	89 45 98             	mov    DWORD PTR [ebp-0x68],eax
  70:	89 55 9c             	mov    DWORD PTR [ebp-0x64],edx
  73:	eb 15                	jmp    8a <__ZN9GlobalMUD8CallbackIxIEE6CallMeIPFxxEIiEEExPv+0x8a>
  75:	8d 6d 18             	lea    ebp,[ebp+0x18]
  78:	8b 45 ac             	mov    eax,DWORD PTR [ebp-0x54]
  7b:	89 04 24             	mov    DWORD PTR [esp],eax
  7e:	c7 45 a8 ff ff ff ff 	mov    DWORD PTR [ebp-0x58],0xffffffff
  85:	e8 00 00 00 00       	call   8a <__ZN9GlobalMUD8CallbackIxIEE6CallMeIPFxxEIiEEExPv+0x8a>
  8a:	8d 45 a4             	lea    eax,[ebp-0x5c]
  8d:	89 04 24             	mov    DWORD PTR [esp],eax
  90:	e8 00 00 00 00       	call   95 <__ZN9GlobalMUD8CallbackIxIEE6CallMeIPFxxEIiEEExPv+0x95>
            #endif
            Private::CallFunction<Function, Arguments...>a(*store);

            delete store;
            return 0;*/
        }
  95:	8b 45 98             	mov    eax,DWORD PTR [ebp-0x68]
  98:	8b 55 9c             	mov    edx,DWORD PTR [ebp-0x64]
  9b:	8d 65 f4             	lea    esp,[ebp-0xc]
  9e:	5b                   	pop    ebx
  9f:	5e                   	pop    esi
  a0:	5f                   	pop    edi
  a1:	5d                   	pop    ebp
  a2:	c3                   	ret    
  a3:	90                   	nop

Disassembly of section .text$_ZN9GlobalMUD8Private215StorageInternalIIiEEC1Ei:

00000000 <__ZN9GlobalMUD8Private215StorageInternalIIiEEC1Ei>:
        template<class T1, class... T2>
        struct StorageInternal<T1,T2...>{
            T1 val;
            StorageInternal<T2...> next;
            static constexpr int type = ((sizeof...(T2))==0)?2:1;
            StorageInternal(T1 v, T2... args) : next(args...){
   0:	55                   	push   ebp
   1:	89 e5                	mov    ebp,esp
   3:	83 ec 04             	sub    esp,0x4
   6:	89 4d fc             	mov    DWORD PTR [ebp-0x4],ecx
   9:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
   c:	83 c0 04             	add    eax,0x4
   f:	89 c1                	mov    ecx,eax
  11:	e8 00 00 00 00       	call   16 <__ZN9GlobalMUD8Private215StorageInternalIIiEEC1Ei+0x16>
                val = v;
  16:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  19:	8b 55 08             	mov    edx,DWORD PTR [ebp+0x8]
  1c:	89 10                	mov    DWORD PTR [eax],edx
            }
  1e:	c9                   	leave  
  1f:	c2 04 00             	ret    0x4
  22:	90                   	nop
  23:	90                   	nop

Disassembly of section .text$_ZN9GlobalMUD8Private212CallFunctionIPFxxExIiEEC1ENS0_7StorageIS3_IiEEE:

00000000 <__ZN9GlobalMUD8Private212CallFunctionIPFxxExIiEEC1ENS0_7StorageIS3_IiEEE>:
        };

        template<class Function, class ReturnType, class... T>
        struct CallFunction{
            ReturnType RetVal;
            CallFunction( Storage<Function, T...> s ){
   0:	55                   	push   ebp
   1:	89 e5                	mov    ebp,esp
   3:	83 ec 38             	sub    esp,0x38
   6:	89 4d f4             	mov    DWORD PTR [ebp-0xc],ecx
                RetVal = IF< s.vals.type < 3, THEN, ELSE>::RESULT::func(s.f, ReturnType(), s.vals);
   9:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
   c:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
   f:	8b 55 10             	mov    edx,DWORD PTR [ebp+0x10]
  12:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  16:	89 54 24 10          	mov    DWORD PTR [esp+0x10],edx
  1a:	c7 44 24 04 00 00 00 	mov    DWORD PTR [esp+0x4],0x0
  21:	00 
  22:	c7 44 24 08 00 00 00 	mov    DWORD PTR [esp+0x8],0x0
  29:	00 
  2a:	89 0c 24             	mov    DWORD PTR [esp],ecx
  2d:	e8 00 00 00 00       	call   32 <__ZN9GlobalMUD8Private212CallFunctionIPFxxExIiEEC1ENS0_7StorageIS3_IiEEE+0x32>
  32:	8b 4d f4             	mov    ecx,DWORD PTR [ebp-0xc]
  35:	89 01                	mov    DWORD PTR [ecx],eax
  37:	89 51 04             	mov    DWORD PTR [ecx+0x4],edx
            }
  3a:	c9                   	leave  
  3b:	c2 0c 00             	ret    0xc
  3e:	90                   	nop
  3f:	90                   	nop

Disassembly of section .text$_ZN9GlobalMUD8Private24THEN4funcIPFxxExIiEIEEET0_T_S5_NS0_15StorageInternalIIDpT1_EEEDpT2_:

00000000 <__ZN9GlobalMUD8Private24THEN4funcIPFxxExIiEIEEET0_T_S5_NS0_15StorageInternalIIDpT1_EEEDpT2_>:
        };

        struct THEN
        {
            template<class Function, class ReturnType, class... Arguments, class... Args2>
            static ReturnType func(Function f, ReturnType r, StorageInternal<Arguments...> s, Args2... args)
   0:	55                   	push   ebp
   1:	89 e5                	mov    ebp,esp
   3:	83 ec 38             	sub    esp,0x38
   6:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
   9:	89 45 f0             	mov    DWORD PTR [ebp-0x10],eax
   c:	8b 45 10             	mov    eax,DWORD PTR [ebp+0x10]
   f:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax
            {
                //If you are seeing an error here, you are probably passing in an incorrect number of arguments to the function.
                return IF< s.type == 1, THEN, ELSE>::RESULT::func(f, ReturnType(), s.next, args..., s.val );
  12:	8b 45 14             	mov    eax,DWORD PTR [ebp+0x14]
  15:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  19:	88 54 24 0c          	mov    BYTE PTR [esp+0xc],dl
  1d:	c7 44 24 04 00 00 00 	mov    DWORD PTR [esp+0x4],0x0
  24:	00 
  25:	c7 44 24 08 00 00 00 	mov    DWORD PTR [esp+0x8],0x0
  2c:	00 
  2d:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  30:	89 04 24             	mov    DWORD PTR [esp],eax
  33:	e8 00 00 00 00       	call   38 <__ZN9GlobalMUD8Private24THEN4funcIPFxxExIiEIEEET0_T_S5_NS0_15StorageInternalIIDpT1_EEEDpT2_+0x38>
            }
  38:	c9                   	leave  
  39:	c3                   	ret    
  3a:	90                   	nop
  3b:	90                   	nop

Disassembly of section .text$_ZN9GlobalMUD8Private24ELSE4funcIPFxxExIEIiEEET0_T_S5_NS0_15StorageInternalIIDpT1_EEEDpT2_:

00000000 <__ZN9GlobalMUD8Private24ELSE4funcIPFxxExIEIiEEET0_T_S5_NS0_15StorageInternalIIDpT1_EEEDpT2_>:


        struct ELSE
        {
            template<class Function, class ReturnType, class... Arguments, class... Args2>
            static ReturnType func(Function f, ReturnType r, StorageInternal<Arguments...> s, Args2... args)
   0:	55                   	push   ebp
   1:	89 e5                	mov    ebp,esp
   3:	83 ec 28             	sub    esp,0x28
   6:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
   9:	89 45 f0             	mov    DWORD PTR [ebp-0x10],eax
   c:	8b 45 10             	mov    eax,DWORD PTR [ebp+0x10]
   f:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax
            {
                //If you are seeing an error here, you are probably passing in an incorrect number of arguments to the function.
                return f(args...);
  12:	8b 45 18             	mov    eax,DWORD PTR [ebp+0x18]
  15:	99                   	cdq    
  16:	89 04 24             	mov    DWORD PTR [esp],eax
  19:	89 54 24 04          	mov    DWORD PTR [esp+0x4],edx
  1d:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  20:	ff d0                	call   eax
            }
  22:	c9                   	leave  
  23:	c3                   	ret    
