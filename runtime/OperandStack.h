//
// Created by zhukovasky on 2020/8/11.
//

#ifndef JAVAP_OPERANDSTACK_H
#define JAVAP_OPERANDSTACK_H

#include <stack>
#include <utility>
#include <vector>
#include "JavaType.h"
#include "iostream"
#include "Slots.h"

namespace Runtime{
    class OperandStack {

    public:

        static OperandStack* newOperandStack(int maxSize);

        void pushInt(int value);

        int popInt();

        void pushLong(long value);

        long popLong();

        void pushFloat(float value);

        float popFloat();

        void pushDouble(double value);

        double popDouble();

        void pushRef(Object* refs);

        Object* popObject();

        void pushBoolean(bool booleans);

        bool popBoolean();

        Slots* popSlots();

        void pushSlots(Slots* slots);

        /** 丢弃栈上全部槽（用于异常 handler 入口清空操作数栈）。 */
        void clear();

        Object* getRefFromTop(long index);

        /** 实例/接口调用的 objectref 位于操作数栈底部连续 `totalInvokeSlots` 槽中的最底槽（局部 0 对应位置）。 */
        Object* peekReceiverForInvoke(int totalInvokeSlots) const;

        template <typename Visitor>
        void visitObjectRefs(Visitor&& visitor) const {
            const int n = size;
            for (int i = 0; i < n; ++i) {
                Slots* slot = innerSlotsList[static_cast<size_t>(i)];
                if (!slot) {
                    continue;
                }
                if (slot->getSlotType() == "ref") {
                    Object* r = slot->getRefs();
                    if (r) {
                        visitor(r);
                    }
                }
            }
        }

    private:

        std::vector<Slots*> innerSlotsList;
        int size;
        int maxSize;
    };
}



#endif //JAVAP_OPERANDSTACK_H
