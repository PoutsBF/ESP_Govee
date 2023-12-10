#include "Govee.h"

bool operator==(const Govee &l, const Govee &r)
{
    return (r.m_adresse == l.m_adresse);
}

bool operator==(const BLEAddress &l, const Govee &r)
{
    return (r.m_adresse->equals(l));
}