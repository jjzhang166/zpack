// VersionDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "zpEditor.h"
#include "VersionDialog.h"
#include "afxdialogex.h"
#include "zpack.h"

// VersionDialog 对话框

IMPLEMENT_DYNAMIC(VersionDialog, CDialogEx)

VersionDialog::VersionDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(VersionDialog::IDD, pParent), m_zpPackage(NULL)
{

}

VersionDialog::~VersionDialog()
{
}

void VersionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_currentVerisonCtrl);
	DDX_Control(pDX, IDC_IPADDRESS2, m_oldVersionCtrl);
}


BEGIN_MESSAGE_MAP(VersionDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &VersionDialog::OnBtnOk)
END_MESSAGE_MAP()


// VersionDialog 消息处理程序


void VersionDialog::OnBtnOk()
{
	if (!m_zpPackage)
		return;

	DWORD dwCurrentVersion, dwOldVersion;
	m_currentVerisonCtrl.GetAddress(dwCurrentVersion);
	m_oldVersionCtrl.GetAddress(dwOldVersion);

	unsigned char mainVersion = (dwCurrentVersion >> 16) & 0xff;
	int subVerion = dwCurrentVersion & 0xffff;

	int oldSubVersion = dwOldVersion & 0xffff;

	if (oldSubVersion <= subVerion)
	{
		m_zpPackage->setMainVersion(mainVersion);
		m_zpPackage->setSubVersion(subVerion);
		m_zpPackage->setOldSubVersion(oldSubVersion);
		m_zpPackage->flush();
	}

	CDialogEx::OnOK();
}


BOOL VersionDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (m_zpPackage)
	{
		unsigned char mainVersion = m_zpPackage->getMainVersion();
		int subVersion = m_zpPackage->getSubVersion();
		int oldVersion = m_zpPackage->getOldSubVersion();

		DWORD dwCurrentAddress = mainVersion << 16;
		dwCurrentAddress += subVersion;
		m_currentVerisonCtrl.SetAddress(dwCurrentAddress);
		m_oldVersionCtrl.SetAddress((mainVersion << 16) + oldVersion);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
