#ifndef INI_H_INCLUDED
#define INI_H_INCLUDED

#include "typedef.h"


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class cNode {
public:
	enum NodeType{ NODE_NONE, NODE_COMMENT, NODE_SECTION, NODE_ENTRY };
	
private:
	cNode *PrevNode, *NextNode;
	
public:
	cNode( cNode * );			// �R���X�g���N�^
	~cNode();					// �f�X�g���N�^
	
	cNode *AddNode();			// �m�[�h��ǉ�
	
	void SetMember( NodeType, const char * );
	
	NodeType NodeID;	// Node ID
	char *Comment;		// Comments
	char *Section;		// Sections
	char *Entry;		// Entries
	char *Value;
	
	cNode *GetPrevNode(){ return PrevNode; }
	cNode *GetNextNode(){ return NextNode; }
	void SetPrevNode( cNode *node ){ PrevNode = node; }
	void SetNextNode( cNode *node ){ NextNode = node; }
	
};

class cIni {
protected:
	cNode *IniNode;				// �擪�m�[�h�ւ̃|�C���^
	char FileName[PATH_MAX];	// �t�@�C����
	
	bool Ready;
	
public:
	cIni();						// �R���X�g���N�^
	virtual ~cIni();			// �f�X�g���N�^
	
	bool Init( const char * );	// ������
	
	bool IsReady(){ return Ready; }
	
	bool Write();															// ������
	bool GetString( const char *, const char *, char *, const char * );		// ������Ǎ���
	bool GetInt( const char *, const char *, int *, const int );			// ���l�Ǎ���
	bool GetTruth( const char *, const char *, bool *, const bool );		// YesNo�Ǎ���
	bool PutEntry( const char *, const char *, const char *, const char *, ... );	// �G���g���ǉ�
	bool DeleteBefore( const char *, const char * );						// �G���g���폜(�O)
	bool DeleteAfter( const char *, const char * );							// �G���g���폜(��)
	const char *GetFileName();												// �t�@�C�����擾
	
};



// �ǂ��ł�SAVE�p�C���^�[�t�F�C�X
struct IDoko
{
	virtual bool DokoLoad( cIni * ) = 0;
	virtual bool DokoSave( cIni * ) = 0;
};

#endif	// INI_H_INCLUDED
