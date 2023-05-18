#include "ReserveCloseRegKey.h"

ReserveCloseRegKey::ReserveCloseRegKey() {

}
ReserveCloseRegKey::ReserveCloseRegKey(HKEY key) {
    reset(key);
}
ReserveCloseRegKey::~ReserveCloseRegKey() {
    if (m_key != NULL) {
        RegCloseKey(m_key);
        m_key == NULL;
    }
}

void ReserveCloseRegKey::reset(HKEY key) {
    if (m_key != NULL) {
        RegCloseKey(m_key);
        m_key == NULL;
    }
    m_key = key;
}