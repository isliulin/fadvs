#pragma once

#define  GM_GUEST		0
#define	 GM_PERSONNEL	1
#define  GM_ADMIN		2
#define  GM_ROOT		3

struct USERDATA 
{
	int		uID;
	int		level;				//0.none 1.guest 2.personnel 3.admin 4.root user
	char	UserName[20];
	char	Password[20];
};

class CUserManage
{
public:
	CUserManage(void);
	~CUserManage(void);
	BOOL		LoadUserData();
	BOOL		FreeUserData();
	BOOL		AddUser(USERDATA user);
	BOOL		DelUser(USERDATA user);
	BOOL		ChangeUser(USERDATA user);
	int			FindUser(USERDATA user);
	USERDATA	m_CurUD;

	char		Switchstr(char ch);
	int			Ln_char2int(char ch,int dr);
	char		Un_int2char(int ich,int dr);
	char		Getrandomstr();
	BOOL		SaveUserData();
private:
	USERDATA	*p_UD;
	CArray		<USERDATA, USERDATA&> m_pUserData;
};