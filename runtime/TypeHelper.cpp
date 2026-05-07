//
// JVM 赋值兼容判断
//

#include "TypeHelper.h"
#include "ClassLoader.h"
#include "heap/JavaClass.h"

#include <string>

namespace Runtime {

static bool isPrimitiveComponentChar(char c) {
    return c == 'Z' || c == 'B' || c == 'S' || c == 'C' || c == 'I' || c == 'J' || c == 'F' || c == 'D';
}

namespace {

bool implementsInterface(JavaClass* val, JavaClass* iface) {
    if (!val || !iface) {
        return false;
    }
    for (JavaClass* it : val->getInterfaces()) {
        if (it == iface) {
            return true;
        }
        if (it && implementsInterface(it, iface)) {
            return true;
        }
    }
    JavaClass* sup = val->getSuperClass();
    return sup && implementsInterface(sup, iface);
}

bool isJvmAssignableNonArray(JavaClass* v, JavaClass* t, ClassLoader* cl) {
    (void)cl;
    if (!v || !t) {
        return false;
    }
    if (v == t) {
        return true;
    }
    if (t->isInterface()) {
        return implementsInterface(v, t);
    }
    for (JavaClass* p = v; p; p = p->getSuperClass()) {
        if (p == t) {
            return true;
        }
    }
    return false;
}

bool arrayNameAssignable(const std::string& v, const std::string& t, ClassLoader* cl) {
    if (v == t) {
        return true;
    }
    if (v.empty() || t.empty() || v[0] != '[' || t[0] != '[') {
        return false;
    }
    const std::string vc = v.substr(1);
    const std::string tc = t.substr(1);
    if (vc == tc) {
        return true;
    }
    if (!vc.empty() && !tc.empty() && vc[0] == '[' && tc[0] == '[') {
        return arrayNameAssignable(vc, tc, cl);
    }
    if (vc.size() == 1 && tc.size() == 1 && isPrimitiveComponentChar(vc[0]) && isPrimitiveComponentChar(tc[0])) {
        return vc[0] == tc[0];
    }
    if (!vc.empty() && !tc.empty() && vc[0] == 'L' && tc[0] == 'L') {
        const size_t semiV = vc.find(';');
        const size_t semiT = tc.find(';');
        if (semiV == std::string::npos || semiT == std::string::npos) {
            return false;
        }
        const std::string nv = vc.substr(1, semiV - 1);
        const std::string nt = tc.substr(1, semiT - 1);
        JavaClass* jv = cl->loadClass(nv);
        JavaClass* jt = cl->loadClass(nt);
        if (!jv || !jt) {
            return false;
        }
        return isJvmAssignableNonArray(jv, jt, cl);
    }
    return false;
}

} // namespace

JavaClass* arrayComponentJavaClass(JavaClass* arrayClass, ClassLoader* loader) {
    if (!arrayClass || !loader || !arrayClass->isArray()) {
        return nullptr;
    }
    const std::string& n = arrayClass->getThisClassName();
    if (n.size() < 2) {
        return nullptr;
    }
    const std::string comp = n.substr(1);
    if (comp.empty()) {
        return nullptr;
    }
    if (comp[0] == '[') {
        return loader->loadClass(comp);
    }
    if (comp[0] == 'L') {
        const size_t semi = comp.find(';');
        if (semi == std::string::npos) {
            return nullptr;
        }
        return loader->loadClass(comp.substr(1, semi - 1));
    }
    if (comp.size() == 1 && isPrimitiveComponentChar(comp[0])) {
        return loader->loadClass(comp);
    }
    return nullptr;
}

bool isJvmAssignable(JavaClass* valueClass, JavaClass* targetClass, ClassLoader* loader) {
    if (!valueClass || !targetClass || !loader) {
        return false;
    }
    if (valueClass == targetClass) {
        return true;
    }
    if (valueClass->isArray() && targetClass->isArray()) {
        return arrayNameAssignable(valueClass->getThisClassName(), targetClass->getThisClassName(), loader);
    }
    if (valueClass->isArray() && !targetClass->isArray()) {
        const std::string& tn = targetClass->getThisClassName();
        if (tn == "java/lang/Object") {
            return true;
        }
        if (tn == "java/io/Serializable" || tn == "java/lang/Cloneable") {
            return true;
        }
        return false;
    }
    if (!valueClass->isArray() && targetClass->isArray()) {
        return false;
    }
    return isJvmAssignableNonArray(valueClass, targetClass, loader);
}

} // namespace Runtime
