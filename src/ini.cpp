#include <stdlib.h>
#include <stdarg.h>

#include "ini.h"
#include "common.h"
#include "osd.h"

#define MAX_LINE	256
#define TABW		4
#define TABN		4


////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
cNode::cNode( cNode *pn ) : PrevNode(pn), NodeID(NODE_NONE),
							Comment(NULL), Section(NULL), Entry(NULL), Value(NULL)
{
	if( PrevNode ){
		NextNode = PrevNode->GetNextNode();
		PrevNode->SetNextNode( this );
	}else
		NextNode = NULL;
}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
cNode::~cNode( void )
{
	if( Comment ) delete [] Comment;
	if( Section ) delete [] Section;
	if( Entry   ) delete [] Entry;
	if( Value   ) delete [] Value;
	
	// �Ȃ����Ă���m�[�h������΍폜
	if( NextNode ) delete NextNode;
}


////////////////////////////////////////////////////////////////
// �m�[�h��ǉ�
////////////////////////////////////////////////////////////////
cNode *cNode::AddNode( void )
{
	return new cNode( this );
}


////////////////////////////////////////////////////////////////
// �f�[�^���Z�b�g(�����O�̕������n��)
////////////////////////////////////////////////////////////////
void cNode::SetMember( NodeType id, const char *str )
{
	char tstr[MAX_LINE+1];
	char *tpstr, *st;
	
	strncpy( tstr, str, MAX_LINE );
	
	NodeID = id;
	switch( NodeID ){
	case NODE_COMMENT:	// Comment
		// ';'�̎��̕������R�����g�̐擪
//		tpstr = strchr( tstr, ';' );
//		if( !tpstr ) tpstr = tstr;
//		else         tpstr++;
		
		// �����̉��s���폜
		while( str[strlen(tstr)-1] == '\n' )
			tstr[strlen(tstr)-1] = '\0';
		
		if( Comment ) delete [] Comment;
		Comment = new char[strlen(tstr)+1];
		strcpy( Comment, tstr );
		break;
		
	case NODE_SECTION:	// Section
		// ']'�̂Ƃ���𖖔��ɂ���
		if( strchr( tstr, ']' ) ) *strchr( tstr, ']' ) = '\0';
		// '['�̎��̕������擪
		if( strchr( tstr, '[' ) ) tpstr = strchr( tstr, '[' ) + 1;
		else                      tpstr = tstr;
		if( Section ) delete [] Section;
		Section = new char[strlen( tpstr ) + 1];
		strcpy( Section, tpstr );
		break;
		
	case NODE_ENTRY:	// Entry
		// �l ---
		// '='�̎��̕������l�̐擪
		tpstr = strchr( tstr, '=' ) + 1;
		
		// �Ōオ���s�Ȃ�'\0'�ɒu��������
		if( tpstr[strlen(tpstr)-1] == '\n' )
			tpstr[strlen(tpstr)-1] = '\0';
		// �l�̑O��' '��'\t'�𖳎�
		while( *tpstr == ' ' || *tpstr == '\t')
			tpstr++;
		
		// �Ō��'\"'�����';'������� ����̓R�����g
		if( strrchr( tpstr, '\"' ) ) st = strchr( strrchr( tpstr, '\"' ), ';' );
		else                         st = strchr( tpstr, ';' );
		if( st ){
			this->SetMember( NODE_COMMENT, st+1 );
			NodeID = NODE_ENTRY;
			*st = '\0';
		}
		
		// �l�̌��' '��'\t'�𖳎�
		while( tpstr[strlen(tpstr)-1] == ' ' || tpstr[strlen(tpstr)-1] == '\t' )
			tpstr[strlen(tpstr)-1] = '\0';
		
		// ����'\"'���΂�
		if( *tpstr == '\"' ) tpstr++;
		// ���'\"'�𖖔��ɂ���
		if( tpstr[strlen(tpstr)-1] == '\"' )
			tpstr[strlen(tpstr)-1] = '\0';
		
		if( Value ) delete [] Value;
		Value = new char[strlen( tpstr ) + 1];
		strcpy( Value, tpstr );
		
		// �G���g�� ---
		// '='�̂Ƃ���𖖔��ɂ���
		*strchr( tstr, '=' ) = '\0';
		tpstr = tstr;
		// �G���g���O��' '��'\t'�𖳎�
		while( *tpstr == ' ' || *tpstr == '\t')
			tpstr++;
		// �G���g�����' '��'\t'�𖳎�
		while( tpstr[strlen(tpstr)-1] == ' ' || tpstr[strlen(tpstr)-1] == '\t' )
			tpstr[strlen(tpstr)-1] = '\0';
		
		if( Entry ) delete [] Entry;
		Entry = new char[strlen( tpstr ) + 1];
		strcpy( Entry, tpstr );
		break;
		
	case NODE_NONE:
	default:
		NodeID = NODE_NONE;
		
	}
	
}


////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
cIni::cIni( void )
{
	Ready   = false;
	IniNode = NULL;
	
	*FileName = '\0';
}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
cIni::~cIni( void )
{
	if( IniNode ) delete IniNode;
}


////////////////////////////////////////////////////////////////
// ������
////////////////////////////////////////////////////////////////
bool cIni::Init( const char *filename )
{
	char str[MAX_LINE+1];
	FILE *fp;
	cNode *node;
	
	// �t�@�C������ۑ�
	strncpy( FileName, filename, PATH_MAX-1 );
	
	// �ŏ��̃m�[�h�m��(�_�~�[)
	if( IniNode ) delete IniNode;
	IniNode = node = new cNode( NULL );
	if( !IniNode ) return false;
	
	// INI�t�@�C�����J��
	fp = FOPENEN( FileName, "rt" );
	if( !fp ) return false;
	
	// �擪����ǂ�
	fseek( fp, 0, SEEK_SET );
	// �f�[�^�������Ȃ�܂ŌJ��Ԃ�
	while( fgets( str, MAX_LINE, fp ) ){
		// �m�[�h�m��
		node = node->AddNode();
		if( !node ){
			fclose( fp );
			return false;
		}
		
		// �擪��';'��������R�����g�s
		if( *str == ';' )
			node->SetMember( cNode::NODE_COMMENT, str+1 );
		else
		// '['��']'���܂�'='���܂܂Ȃ��Ȃ�Z�N�V����
		if( strchr( str, '[' ) && strchr( str, ']' ) && !strchr( str, '=' ) )
			node->SetMember( cNode::NODE_SECTION, str );
		else
		// '='���܂ނȂ�G���g��
		if( strchr( str, '=' ) )
			node->SetMember( cNode::NODE_ENTRY, str );
		else
		// �ǂ�ł��Ȃ���΃R�����g
			node->SetMember( cNode::NODE_COMMENT, str );
	}
	fclose( fp );
	
	Ready = true;
	
	return true;
}


////////////////////////////////////////////////////////////////
// ������
////////////////////////////////////////////////////////////////
bool cIni::Write( void )
{
	FILE *fp;
	cNode *node;
	
	if( !Ready ) return false;
	
	// INI�t�@�C�����J��
	fp = FOPENEN( FileName, "wt" );
	if( !fp ) return false;
	
	node = IniNode;
	do{
		switch( node->NodeID ){
		case cNode::NODE_COMMENT:	// Comment
			if( strlen(node->Comment) ) fprintf( fp, ";%s\n", node->Comment );
			else                        fprintf( fp, "\n" );
			break;
			
		case cNode::NODE_SECTION:	// Section
			fprintf( fp, "[%s]\n", node->Section );
			break;
			
		case cNode::NODE_ENTRY:	// Entry
			fprintf( fp, "%s", node->Entry );
			for( int i=TABN*TABW-(int)strlen(node->Entry); i>0; i-=TABW )
				fprintf( fp, "\t" );
				
			fprintf( fp, "= %s", node->Value );
				
			if( node->Comment ){
				for( int i=TABN*TABW-((int)strlen(node->Value)+2); i>0; i-=TABW )
					fprintf( fp, "\t" );
				fprintf( fp, "\t;%s\n", node->Comment );
			}else
				fprintf( fp, "\n" );
			
			break;
			
		case cNode::NODE_NONE:
		default:
			break;
		}
	}while( (node = node->GetNextNode()) );
	
	fclose( fp );
	
	return true;
}


////////////////////////////////////////////////////////////////
// ������Ǎ���
////////////////////////////////////////////////////////////////
bool cIni::GetString( const char *section, const char *entry, char *val, const char *def )
{
	cNode *node, *tnode;
	bool Found  = false;
	
	// �܂��̓f�t�H���g�l���Z�b�g
	strcpy( val, def );
	
	if( !Ready ) return false;
	
	// �Z�N�V������T��
	node = IniNode;
	do{
		if( node->NodeID == cNode::NODE_SECTION )
			if( !stricmp( node->Section, section ) ) Found = true;
	}while( (node = node->GetNextNode()) && !Found );
	// �Z�N�V������������Ȃ����G���g�����Ȃ���΃G���[
	if( !Found || !node ) return false;
	Found = false;
	
	// �G���g����T��
	do{
		tnode = node;
		if( node->NodeID == cNode::NODE_ENTRY )
			if( !stricmp( node->Entry, entry ) ) Found = true;
	}while( (node = node->GetNextNode()) && tnode->NodeID != cNode::NODE_SECTION && !Found  );
	// �G���g����������Ȃ���΃G���[
	if( !Found ) return false;
	
	// �l��ۑ�
	strcpy( val, tnode->Value );
	
	return true;
}


////////////////////////////////////////////////////////////////
// ���l�Ǎ���
////////////////////////////////////////////////////////////////
bool cIni::GetInt( const char *section, const char *entry, int *val, const int def )
{
	bool res;
	char str[MAX_LINE+1];
	
	// �܂��̓f�t�H���g�l���Z�b�g
	*val = def;
	
	if( (res = GetString( section, entry, str, "" )) )
		*val = strtol( str, NULL, 0 );
	
	return res;
}


////////////////////////////////////////////////////////////////
// YesNo�Ǎ���
////////////////////////////////////////////////////////////////
bool cIni::GetTruth( const char *section, const char *entry, bool *val, const bool def )
{
	bool res;
	char str[MAX_LINE+1];
	
	// �܂��̓f�t�H���g�l���Z�b�g
	*val = def;
	
	if( (res = GetString( section, entry, str, "" )) ){
		if( !strcmp( str, "1" ) || !stricmp( str, "yes" ) || !stricmp( str, "on" ) || !stricmp( str, "true" ) )
			*val = true;
		else
		if( !strcmp( str, "0" ) || !stricmp( str, "no" ) || !stricmp( str, "off" ) || !stricmp( str, "false" ) )
			*val = false;
		else
			return false;
	}
	return res;
}


////////////////////////////////////////////////////////////////
// �G���g���ǉ�
////////////////////////////////////////////////////////////////
bool cIni::PutEntry( const char *section, const char *comment, const char *entry, const char *val, ... )
{
	char rstr[MAX_LINE+1];
	cNode *node, *tnode;
	bool Found = false;
	
	if( !Ready ) return false;
	
	// ��������
	va_list arg;
	va_start( arg, val );
	vsprintf( rstr, val, arg );
	va_end( arg );
	
	
	// �Z�N�V������T��
	node = IniNode;
	do{
		tnode = node;
		if( node->NodeID == cNode::NODE_SECTION )
			if( !stricmp( node->Section, section ) ) Found = true;
	}while( (node = node->GetNextNode() ) && !Found );
	
	// �Z�N�V������������Ȃ���Βǉ�
	if( !Found ){
		// �Z�N�V�����m�[�h�ǉ�
		node = tnode->AddNode();
		if( !node ) return false;
		node->SetMember( cNode::NODE_SECTION, section );
		// �G���g���m�[�h�ǉ�
		node = node->AddNode();
		if( !node ) return false;
	}else{
	// �Z�N�V���������������ꍇ
		Found = false;
		
		// �G���g����T��
		do{
			tnode = node;
			if( node->NodeID == cNode::NODE_ENTRY )
				if( !stricmp( node->Entry, entry ) ) Found = true;
		}while( (node = node->GetNextNode()) && tnode->NodeID != cNode::NODE_SECTION && !Found  );
		node = tnode;
		// �G���g����������Ȃ���Βǉ�
		if( !Found ){
			// ���̃Z�N�V�����ɒB�����ꍇ�͑O�Z�N�V�����̍Ō�̃m�[�h��I��
			if( tnode->NodeID == cNode::NODE_SECTION ) node = tnode->GetPrevNode();
			
			// �G���g���m�[�h�ǉ�
			node = node->AddNode();
			if( !node ) return false;
		}
	}
	
	// �G���g���ɃR�����g����Ȃ�ǉ�
	if( comment ) node->SetMember( cNode::NODE_COMMENT, comment );
	
	// �G���g����ۑ�
	char tstr[MAX_LINE+1];
	sprintf( tstr, "%s=%s", entry, rstr );
	node->SetMember( cNode::NODE_ENTRY, tstr );
	
	return true;
}


////////////////////////////////////////////////////////////////
// �G���g���폜(�O)
////////////////////////////////////////////////////////////////
bool cIni::DeleteBefore( const char *section, const char *entry )
{
	cNode *node, *tnode;
	bool Found = false;
	
	if( !Ready ) return false;
	
	// �Z�N�V������T��
	node = IniNode;
	do{
		tnode = node;
		if( node->NodeID == cNode::NODE_SECTION )
			if( !stricmp( node->Section, section ) ) Found = true;
	}while( (node = node->GetNextNode() ) && !Found );
	
	// �Z�N�V������������Ȃ���Ή������Ȃ�
	if( !Found ){
		return false;
	} else {
		// �Z�N�V���������������ꍇ
		Found = false;
		
		// �G���g����T��
		do{
			tnode = node;
			if( node->NodeID == cNode::NODE_ENTRY )
				if( !stricmp( node->Entry, entry ) ) Found = true;
		}while( (node = node->GetNextNode()) && tnode->NodeID != cNode::NODE_SECTION && !Found  );
		node = tnode;
		
		// �G���g����������Ȃ���Ή������Ȃ�
		if( !Found ){
			return false;
		} else {
			// �G���g�������������ꍇ�͂�����O���폜(�w�肳�ꂽ�G���g�����܂�)
			tnode = node;
			do{
				tnode = tnode->GetPrevNode();
			}while( tnode && tnode->NodeID != cNode::NODE_SECTION );
			tnode->GetPrevNode()->SetNextNode(node->GetNextNode());
			node->GetPrevNode()->SetNextNode(NULL);
			delete tnode;
		}
	}
}


////////////////////////////////////////////////////////////////
// �G���g���폜(��)
////////////////////////////////////////////////////////////////
bool cIni::DeleteAfter( const char *section, const char *entry )
{
	cNode *node, *tnode;
	bool Found = false;
	
	if( !Ready ) return false;
	
	// �Z�N�V������T��
	node = IniNode;
	do{
		tnode = node;
		if( node->NodeID == cNode::NODE_SECTION )
			if( !stricmp( node->Section, section ) ) Found = true;
	}while( (node = node->GetNextNode() ) && !Found );
	
	// �Z�N�V������������Ȃ���Ή������Ȃ�
	if( !Found ){
		return false;
	} else {
		// �Z�N�V���������������ꍇ
		Found = false;
		
		// �G���g����T��
		do{
			tnode = node;
			if( node->NodeID == cNode::NODE_ENTRY )
				if( !stricmp( node->Entry, entry ) ) Found = true;
		}while( (node = node->GetNextNode()) && tnode->NodeID != cNode::NODE_SECTION && !Found  );
		node = tnode;
		
		// �G���g����������Ȃ���Ή������Ȃ�
		if( !Found ){
			return false;
		} else {
			// �G���g�������������ꍇ�͂��������폜(�w�肳�ꂽ�G���g�����܂�)
			tnode = node;
			do{
				tnode = tnode->GetNextNode();
			}while( tnode && tnode->NodeID != cNode::NODE_SECTION );
			if(tnode){
				tnode->GetPrevNode()->SetNextNode(NULL);
			}
			node->GetPrevNode()->SetNextNode(tnode);
			delete node;
		}
	}
}


////////////////////////////////////////////////////////////////
// �t�@�C�����擾
////////////////////////////////////////////////////////////////
const char *cIni::GetFileName( void  )
{
	return FileName;
}
