#include "pnlTok.hpp"
#include "pnlWGraph.hpp"
#include "TokenCover.hpp"

#pragma warning(push, 2)
#pragma warning(disable: 4251)
// class X needs to have dll-interface to be used by clients of class Y
#include "pnl_dll.hpp"
#pragma warning(default: 4251)
#pragma warning(pop)
#include "WInner.hpp"

#if defined(_MSC_VER)
#pragma warning(disable : 4239) // nonstandard extension used: 'T' to 'T&'
#endif

TokenCover::TokenCover(const char *rootName, WGraph *graph, bool bAutoNum): m_pGraph(graph)
{
    CreateRoot(rootName, bAutoNum);

    (m_aNode = m_pRoot->Add("nodes"))->tag = eTagService;
    (m_pProperties = m_pRoot->Add("properties"))->tag = eTagService;
    (m_pCategoric  = m_aNode->Add("categoric")) ->tag = eTagNodeType;
    (m_pContinuous = m_aNode->Add("continuous"))->tag = eTagNodeType;
    m_pDefault = m_pCategoric;
    SpyTo(m_pGraph);
}

TokenCover::TokenCover(TokIdNode *root, WGraph *graph):
    m_pRoot(root), m_pGraph(graph)
{
    m_aNode = Tok("nodes").Node(root);
    m_pCategoric  = Tok("categoric").Node(m_aNode);
    //if(!m_pCategoric || m_pCategoric->tag != eTagNodeType) throw();
    m_pContinuous = Tok("continuous").Node(m_aNode);
    //if(!m_pContinuous || m_pContinuous->tag != eTagNodeType) throw();
    m_pProperties = Tok("properties").Node(m_pRoot);
    //if(!m_pProperties || m_pProperties->tag != eTagService) throw();
    SpyTo(m_pGraph);
}

TokenCover::TokenCover(const char *rootName, const TokenCover &tokCovFrom, bool bAutoNum)
{
    CreateRoot(rootName, bAutoNum);
    if(!CopyRecursive(m_pRoot, tokCovFrom.m_pRoot))
    {
	ThrowInternalError("error during Token recursive coping",
	    "TovenCover::TocenCover");
    }
    m_pGraph = new WGraph(*tokCovFrom.m_pGraph);
    SpyTo(m_pGraph);
}

TokenCover::~TokenCover()
{
    m_pRoot->Kill();
}

void TokenCover::CreateRoot(const char *rootName, bool bAutoNum)
{
    TokIdNode *root = flatSearch(TokIdNode::root, rootName);
    if(!root)
    {
	root = TokIdNode::root->Add(rootName);
	root->tag = eTagRootClassification;
    }

    if(bAutoNum)
    {
	// search for first unfilled slot in rootName
	int i;
	pnl::pnlString s;
	for(i = 0; flatSearch(root, (s << i).c_str()); ++i, s.resize(0));
	
	// create substructure
	m_pRoot = root->Add(s.c_str());
    }
    else
    {
	m_pRoot = root;
    }
    m_pRoot->tag = eTagNet;
    m_pRoot->data = this;
}

void TokenCover::Resolve(Tok &tok) const
{
    tok.Resolve(Root());
}

void TokenCover::Resolve(TokArr &aTok) const
{
    for(int i = aTok.size(); --i >= 0; aTok[i].Resolve(Root()));
}

bool TokenCover::CopyRecursive(TokIdNode *to, const TokIdNode *from)
{
    TokIdNode *childFrom, *childTo;

    try
    {
	for(childFrom = from->v_next; childFrom; childFrom = childFrom->h_next)
	{
	    childTo = to->Add(childFrom->id[0]);
	    childTo->tag = childFrom->tag;
	    for(int i = 1; i < childFrom->id.size(); ++i)
	    {
		childTo->Alias(childFrom->id[i]);
	    }
	    if(!CopyRecursive(childTo, childFrom))
	    {
		return false;
	    }
	}
    }
    catch(...)
    {
	return false;
    }

    return true;
}

int TokenCover::AddNode(String &nodeName)
{
    return AddNode(Tok(nodeName), TokArr("True False"));
}

void TokenCover::AddNode(TokArr &nodes, TokArr &values)
{
    int i;

    for(i = 0; i < nodes.size(); i++)
    {
	AddNode(nodes[i], values);
    }
}

int TokenCover::AddNode(Tok &node, TokArr &aValue)
{
    TokIdNode *parentNode = m_pRoot;
    String nodeName = node.Name();
    if(node.Unresolved(parentNode).size() != 1)
    {
        pnl::pnlString str;
        str << '\'' << nodeName << "' is not a node";
        ThrowUsingError(str.c_str(), "AddNode");
    }

    //If token is unresolved, set default type (categoric)
    if(node.node.empty())
    {
	parentNode = m_pDefault;
    }
    else 
    {
	parentNode = node.Node(parentNode);
    }

    // add Bayes vertex
    TokIdNode *tokNode = parentNode->Add(nodeName);
    tokNode->tag = eTagNetNode;
    TokIdNode *pValue;

    for(int j = 0; j < aValue.size(); ++j)
    { // add vertex's values (or dimension names)
	pValue = tokNode->Add(aValue[j].Name());
	TuneNodeValue(pValue, j);
    }

    int result = 0;

    if(m_pGraph)
    {
	result = m_pGraph->AddNode(nodeName);	
	tokNode->Alias(result);
    }

    return result;
}

bool TokenCover::DelNode(int iNode)
{
    Notify(eDelNode, iNode);
    if(!m_pGraph)
    {
	ThrowInternalError("Call to TokenCover::DelNode without graph", "DelNode");
	return false;
    }

    TokIdNode *node = Tok(m_pGraph->NodeName(iNode)).Node(m_aNode);
    if(!node)
    {
	return false;
    }
    m_pGraph->DelNode(iNode);
    node->Kill();

    return true;
}

bool TokenCover::DelNode(Tok &nodeName)
{
    String name(nodeName.Name());

    TokIdNode *node = nodeName.Node(m_aNode);

    if(!node)
    {
	return false;
    }

    node->Kill();

    if(m_pGraph && !m_pGraph->DelNode(m_pGraph->INode(name.c_str())))
    {
	return false;
    }

    return true;
}

Vector<TokIdNode*> TokenCover::Nodes(Vector<int> aiNode)
{
    Vector<TokIdNode *> result;

    result.resize(aiNode.size());
    for(int i = aiNode.size(); --i >= 0;)
    {
	result[i] = Node(aiNode[i]);
    }
    return result;
}

int TokenCover::nValue(int iNode)
{
    TokIdNode *value = Node(iNode)->v_next;
    int i;

    for(i = 0; value; value = value->h_next, ++i);

    return i;
}

TokIdNode *TokenCover::Node(int iNode) const
{
    if(!m_pGraph)
    {
	ThrowInternalError("This call requires graph", "Node");
	return 0;
    }

    return Tok(m_pGraph->NodeName(iNode)).Node(m_aNode);
}

TokIdNode *TokenCover::Node(Tok &node) const
{
    return node.Node(m_aNode);
}

void TokenCover::SetValues(int iNode, const Vector<String> &aValue)
{
    SetValues(Node(iNode), aValue, iNode);
}

void TokenCover::SetValues(TokIdNode *node, const Vector<String> &aValue, int iNode)
{
    TokIdNode *pValue;

    KillChildren(node);
    for(int j = 0; j < aValue.size(); ++j)
    { // add vertex's values (or dimension names)
	pValue = node->Add(aValue[j]);
	TuneNodeValue(pValue, j);
    }
    Notify(eChangeNState, iNode);
}

void TokenCover::SetValue(int iNode, int iValue, String &value)
{
}

TokIdNode *TokenCover::NodeValue(int iNode, int iValue) const
{
#ifndef SEARCH_IN_NODE
    TokIdNode *node = Node(iNode);
    
    node = node->v_next;
    for(; node; node = node->h_next)
    {
	if(node->Match(TokId(iValue)))
	{
	    return node;
	}
    }

    return 0;
#else
    return Tok(iValue).Node(Node(iNode));
#endif
}

void TokenCover::TuneNodeValue(TokIdNode *pValue, int iValue)
{
    pValue->tag = eTagValue;
    pValue->Alias(iValue);
}

void TokenCover::KillChildren(TokIdNode *node)
{
    Vector<TokIdNode *> children;
    int i;

    children.reserve(16);
    for(i = 0, node = node->v_next; node; node = node->h_next)
    {
	children.push_back(node);
    }

    for(i = children.size(); --i >= 0; children[i]->Kill());
}

String TokenCover::Value(int iNode, int iValue) const
{
    return NodeValue(iNode, iValue)->Name();
}

void TokenCover::GetValues(int iNode, Vector<String> &aValue)
{
    TokIdNode *node = Node(iNode);
    TokIdNode *value = node->v_next;
    int i;

    for(i = 0; value; value = value->h_next, ++i);
    aValue.resize(i);
    for(value = node->v_next; --i >= 0; value = value->h_next)
    {
	aValue[i] = value->Name();
    }
}

Vector<TokIdNode*> TokenCover::ExtractNodes(TokArr &aValue) const
{
    Vector<TokIdNode*> result;
    int j;
    for(int i = 0; i < aValue.size(); ++i)
    {
	int j = TokIdNode::root->desc.count(aValue[i].Name());
	TokIdNode *node = aValue[i].Node(m_aNode);
	if(node->tag == eTagValue)
	{
	    node = node->v_prev;
	}
	if(node->tag != eTagNetNode)
	{
	    ThrowUsingError("There is must be node", "ExtractNodes");
	}
	result.push_back(node);
    }

    return result;
}

int TokenCover::NodesClassification(TokArr &aValue) const
{
    int result = 0;
    int i;
    TokIdNode *node;

    for(i = 0; i < aValue.size(); ++i)
    {
	node = aValue[i].Node(m_aNode);
	while(node && node->tag != eTagNodeType)
	{
	    node = node->v_prev;
	}
	if(!node)
	{
	    result |= eNodeClassUnknown;
	    continue;
	}
	if(node == m_pCategoric)
	{
	    result |= eNodeClassCategoric;
	}
	else if(node == m_pContinuous)
	{
	    result |= eNodeClassContinuous;
	}
	else
	{
	    ThrowInternalError("Non-consistent node classification", "NodesClassification");
	}
    }

    return result;
}

void TokenCover::RenameGraph(const int *renaming)
{
    ThrowInternalError("not yet realized", "RenameGraph");
}

void TokenCover::AddProperty(const char *name, const char **aValue, int nValue)
{
    TokIdNode *node = m_pProperties->Add(name);

    for(int i = 0; i < nValue; ++i)
    {
	node->Add(aValue[i]);
    }
}

void TokenCover::GetPropertyVariants(const char *name, Vector<String> &aValue) const
{
    TokIdNode *node = Tok(name).Node();

    aValue.resize(0);
    if(!node || !(node = node->v_next))
    {
	return;
    }
    aValue.reserve(8);

    for(; node; node = node->h_next)
    {
	aValue.push_back(node->Name());
    }
}

void TokenCover::DoNotify(int message, int iNode, ModelEngine *pObj)
{
    switch(message)
    {
    case eChangeName:
	{
	    PNL_CHECK_IS_NULL_POINTER(m_pGraph);
	    TokIdNode *node = Node(iNode);
	    String name = m_pGraph->NodeName(iNode);
	    String oldName = node->Name();

	    node->Alias(name);
	    node->Unalias(oldName);
	    if(!(node->Name() == name))
	    {
		node->Unalias(iNode);
		node->Alias(iNode);
	    }
	    if(!(node->Name() == name))
	    {
		ThrowInternalError("can't rename node with token", "Notify::eChangeName");
		node->Unalias(iNode);
		node->Alias(iNode);
	    }
	}
	break;
    case eDelNode:
    default:
	ThrowInternalError("Unhandled message arrive" ,"DoNotify");
	return;
    }
}