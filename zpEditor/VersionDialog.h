#pragma once
#include "afxwin.h"
#include "afxcmn.h"

namespace zp{
	class IPackage;
};

// VersionDialog �Ի���

class VersionDialog : public CDialogEx
{
	DECLARE_DYNAMIC(VersionDialog)

public:
	VersionDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~VersionDialog();

// �Ի�������
	enum { IDD = IDD_DLG_VERSION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBtnOk();

public:
	void setPackage(zp::IPackage* package){
		m_zpPackage = package;
	}

private:
	CIPAddressCtrl m_currentVerisonCtrl;
	CIPAddressCtrl m_oldVersionCtrl;

	zp::IPackage* m_zpPackage;
public:
	virtual BOOL OnInitDialog();
};
