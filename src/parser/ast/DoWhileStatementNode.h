/*
 * Copyright (c) 2016-present Samsung Electronics Co., Ltd
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 */

#ifndef DoWhileStatementNode_h
#define DoWhileStatementNode_h

#include "ExpressionNode.h"
#include "StatementNode.h"

namespace Escargot {

class DoWhileStatementNode : public StatementNode {
public:
    friend class ScriptParser;
    DoWhileStatementNode(Node *test, Node *body)
        : StatementNode()
    {
        m_test = (ExpressionNode *)test;
        m_body = (StatementNode *)body;
    }

    virtual ~DoWhileStatementNode()
    {
    }

    virtual ASTNodeType type() { return ASTNodeType::DoWhileStatement; }
    virtual void generateStatementByteCode(ByteCodeBlock *codeBlock, ByteCodeGenerateContext *context)
    {
        ByteCodeGenerateContext newContext(*context);

        size_t doStart = codeBlock->currentCodeSize();
        m_body->generateStatementByteCode(codeBlock, &newContext);

        newContext.getRegister(); // ExeuctionResult of m_body should not be overwritten by m_test
        size_t testPos = codeBlock->currentCodeSize();
        size_t testReg = m_test->getRegister(codeBlock, &newContext);
        m_test->generateExpressionByteCode(codeBlock, &newContext, testReg);
        codeBlock->pushCode(JumpIfTrue(ByteCodeLOC(m_loc.index), testReg, doStart), &newContext, this);

        newContext.giveUpRegister();
        newContext.giveUpRegister();

        size_t doEnd = codeBlock->currentCodeSize();
        newContext.consumeContinuePositions(codeBlock, testPos, context->m_tryStatementScopeCount);
        newContext.consumeBreakPositions(codeBlock, doEnd, context->m_tryStatementScopeCount);
        newContext.m_positionToContinue = testPos;
        newContext.propagateInformationTo(*context);
    }

protected:
    RefPtr<ExpressionNode> m_test;
    RefPtr<StatementNode> m_body;
};
}

#endif
