#if !defined(AFX_SOCKETUDP_H__F7F82E8F_2320_4BC2_8DBA_6809AFF724E5__INCLUDED_)
#define AFX_SOCKETUDP_H__F7F82E8F_2320_4BC2_8DBA_6809AFF724E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SocketUdp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSocketUdp command target

class CSocketUdp : public CSocket
{
// Attributes
public:

// Operations
public:
	CSocketUdp();
	virtual ~CSocketUdp();

// Overrides
public:
	//自定义函数，与对话框相关联
	void SetParent(CDialog  *pWnd);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSocketUdp)
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CSocketUdp)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
private:
	CDialog *m_pWnd;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOCKETUDP_H__F7F82E8F_2320_4BC2_8DBA_6809AFF724E5__INCLUDED_)
