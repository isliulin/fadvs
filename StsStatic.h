#pragma once


// CStsStatic

class CStsStatic : public CStatic
{
	DECLARE_DYNAMIC(CStsStatic)

public:
	CStsStatic();
	virtual ~CStsStatic();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


