import os
import sys
import ntpath
import subprocess
import shlex
import glob
import re



if (ntpath.exists("build")):
	os.system("rmdir /s /q .\\build")
os.mkdir("build")
os.chdir(os.getcwd()+"\\build\\")
ti,tp,os.environ["INCLUDE"],os.environ["PATH"],os.environ["_NO_DEBUG_HEAP"]=os.environ["INCLUDE"],os.environ["PATH"],".\\..\\src\\include;"+os.environ["INCLUDE"],"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\BuildTools\\VC\\Tools\\MSVC\\14.25.28610\\bin\\Hostx64\\x64;"+os.environ["PATH"],"1"
rel=(True if len(sys.argv)==2 and sys.argv[1]=="-r" else False)
s=""
for fn in os.listdir(".\\..\\src\\include"):
	if (fn[-2:]==".h"):
		with open(f".\\..\\src\\include\\{fn}","r") as hf:
			dt=hf.read()
			m=re.fullmatch(r"^\s*#ifndef\s+(?P<h_nm>\w+)\n\s*#define\s+(?P=h_nm)\s*\n(.*)#endif\s*$",dt,re.DOTALL)
			if (m!=None):
				s+=m.group(2)
			else:
				s+=dt
iil=[f"<{fn}>|\\\"{fn}\\\"" for fn in os.listdir(".\\..\\src\\include")]
s=re.sub(r"^[ \t\r]*(enum\s+\w+)\s*({[^}]*})\s*;\s*$",lambda g:g.group(1)+re.sub(r"\s*","",g.group(2))+";",re.sub(r"^[ \t\r]*(#define \w+\([^\)]*\))[ \t\r]*(\\\n(?:[ \t\r]*.*\\\n)+[ \t\r]*.*\n?)",lambda g:g.group(1)+" "+re.sub(r"\\\n[ \t\r]*","",g.group(2)),re.sub(r"\n+","\n",re.sub(rf"(?:^[ \t\f]*#include\s*(?:{'|'.join(iil)})|//[^\n]*)$","",re.sub(r"\/\*.*?\*\/","",s,flags=re.DOTALL),flags=re.M)),flags=re.M),flags=re.M).replace("\r\n","\n").split("\n")
i=0
b=0
r=[False]
p=[]
o=[]
il=[]
pl=[]
d={}
while (i<len(s)):
	s[i]=s[i].strip()
	if (len(s[i].strip())==0):
		i+=1
		continue
	if (s[i].startswith("#ifdef") or s[i].startswith("#ifndef")):
		if (r[-1]==True):
			b+=1
			p+=[False]
		else:
			fd=False
			t=s[i].split(" ")[1]
			nr=(False if s[i].startswith("#ifdef") else True)
			p+=[True]
			r+=[False]
			if (t in [("_DEBUG" if rel==False else "NDEBUG"),"_WINDOWS","_UNICODE","UNICODE","_USRDLL","_WINDLL","DLL1_EXPORTS"]):
				r[-1]=nr
			elif (t in [("_DEBUG" if rel==True else "NDEBUG"),"WIN32"]):
				r[-1]=not nr
			else:
				p[-1]=False
				pl+=[s[i]]
			b+=1
	elif (s[i]=="#else"):
		if (p[-1]==True):
			r[-1]=not r[-1]
		else:
			pl+=["#else"]
	elif (s[i]=="#endif"):
		if (p[-1]==False):
			pl+=["#endif"]
		if (b>0):
			b-=1
			p=p[:-1]
			r=r[:-1]
		else:
			print("WHAT?")
	elif (s[i].startswith("#include")):
		if (r[-1]==False):
			j=8
			while (s[i][j] in " \t\r"):
				j+=1
			if_=s[i][j+1:][:s[i][j+1:].index((">" if s[i][j]=="<" else s[i][j]))]
			if (if_ not in il):
				pl+=[s[i]]
				il+=[if_]
	elif (s[i].startswith("#define")):
		if (r[-1]==False):
			d[s[i].split(" ")[1]]=s[i][9+len(s[i].split(" ")[1]):]
			pl+=[s[i]]
	elif (r[-1]==False):
		if (s[i][0]=="#"):
			pl+=[s[i]]
		else:
			o+=[s[i]]
			ok=False
			while (True):
				for k in d.keys():
					o[-1]=re.sub(rf"\b{k}\b",lambda x:(exec("ok=True"),d[k])[1],o[-1])
				if (ok==False):
					break
	i+=1
i=0
s=o[:]
o=[]
while (i<len(s)):
	if (s[i][0]=="#"):
		o+=[s[i]]
	else:
		if (re.match(r"^(?:const)?\s*(?:static)?\s*(?:const)?\s*struct",s[i])!=None and "{" in s[i] and s[i].count("{")!=s[i].count("}")):
			o+=[s[i]]
			b=s[i].count("{")-s[i].count("}")
			i+=1
			while (b!=0):
				o[-1]+=s[i].strip();
				b+=s[i].count("{")-s[i].count("}")
				i+=1
			i-=1
		else:
			o+=[s[i]]
	i+=1
el=[]
oi=1
for k in re.findall(r"^\w+(?:[ \t\r]+\w+)*\*{0,2}[ \t\r]+(\w+)\(","\n".join(o),re.M):
	el+=[f"/EXPORT:{k},@{oi},NONAME"]
	oi+=1
for k in re.findall(r"^extern(?:[ \t\r]+\w+)+\*{0,2}[ \t\r]+(\w+)[ \t\r]*;[ \t\r]*$","\n".join(o),re.M):
	el+=[f"/EXPORT:{k},@{oi},NONAME,DATA"]
	oi+=1
with open("kl.h","w") as f:
	f.write("#ifndef KL_H\n#define KL_H\n"+"\n".join(pl)+"\n"+"\n".join(o)+"\n#endif")
with open("tmp.c","w") as f:
	f.write("\n".join([f"#include <{fn}>" for fn in os.listdir(".\\..\\src\\include")]))
if (subprocess.run(shlex.split(f"cl /c /permissive- /GS /W3 /Zc:wchar_t /Gm- /sdl /Zc:inline /fp:precise /D \"{('_' if rel==False else 'N')}DEBUG\"  /D \"_WINDOWS\" /D \"_USRDLL\" /D \"_WINDLL\" /D \"_UNICODE\" /D \"UNICODE\" /errorReport:none /WX /Zc:forScope /Gd /Oi /FC /D \"DLL1_EXPORTS\" /EHsc /nologo /diagnostics:column {('/ZI /Od /RTC1 /MDd' if rel==False else '/GL /Gy /Zi /O2 /Oi /MD')} tmp.c ..\\\\src\\\\dllmain.c ..\\\\src\\\\core\\\\platform\\\\windows.c "+' '.join(glob.glob("..\\src\\core\\*.c")+glob.glob("..\\src\\core\\util\\*.c")).replace("\\","\\\\"))).returncode==0 and subprocess.run(shlex.split(f"link {' '.join(glob.glob('*.obj'))} /OUT:kl.dll /IMPLIB:kl.lib /NXCOMPAT /DYNAMICBASE \"kernel32.lib\" \"user32.lib\" \"gdi32.lib\" \"winspool.lib\" \"comdlg32.lib\" \"advapi32.lib\" \"shell32.lib\" \"ole32.lib\" \"oleaut32.lib\" \"uuid.lib\" \"odbc32.lib\" \"odbccp32.lib\" /DLL /MACHINE:X64 /SUBSYSTEM:WINDOWS /ERRORREPORT:none /NOLOGO /TLBID:1 /WX {('/DEBUG /INCREMENTAL' if rel==False else '/LTCG /OPT:REF /INCREMENTAL:NO /OPT:ICF')} {' '.join(el)}")).returncode==0):
		os.environ["INCLUDE"]=".\\;"+ti
		if (subprocess.run(shlex.split(f"cl /c /permissive- /GS /W3 /Zc:wchar_t /Gm- /sdl /Zc:inline /fp:precise /D \"{('_' if rel==False else 'N')}DEBUG\"  /D \"_WINDOWS\" /D \"_USRDLL\" /D \"_WINDLL\" /D \"_UNICODE\" /D \"UNICODE\" /errorReport:none /WX /Zc:forScope /Gd /Oi /FC /D \"DLL1_EXPORTS\" /EHsc /nologo /diagnostics:column {('/ZI /Od /RTC1 /MDd' if rel==False else '/GL /Gy /Zi /O2 /Oi /MD')} ..\\\\src\\\\main.c")).returncode==0 and subprocess.run(shlex.split(f"link main.obj /OUT:kl.exe /DYNAMICBASE \"kernel32.lib\" \"user32.lib\" \"gdi32.lib\" \"winspool.lib\" \"comdlg32.lib\" \"advapi32.lib\" \"shell32.lib\" \"ole32.lib\" \"oleaut32.lib\" \"uuid.lib\" \"odbc32.lib\" \"odbccp32.lib\" \"kl.lib\" /MACHINE:X64 /SUBSYSTEM:CONSOLE /ERRORREPORT:none /NOLOGO /TLBID:1 /WX {('/DEBUG /INCREMENTAL' if rel==False else '/LTCG /OPT:REF /INCREMENTAL:NO /OPT:ICF')}")).returncode==0):
			os.system(f"del tmp.c&&del *obj&&del *.pdb&&del *.exp{('' if rel==True else '&&del *.ilk&&del *.idb')}&&cls")
			subprocess.run(["kl.exe",".\\..\\test.kl"])
os.environ["INCLUDE"],os.environ["PATH"]=ti,tp
os.chdir(os.getcwd()+"\\..\\")
