#include "isxUtilities.h"

namespace isx
{
    size_t getDataTypeSizeInBytes(DataType inDataType)
    {
        switch (inDataType)
        {
            case DataType::U8:
            {
                return sizeof(uint8_t);
            }
            case DataType::U16:
            {
                return sizeof(uint16_t);
            }
            case DataType::F32:
            {
                return sizeof(float);
            }
            case DataType::RGB888:
            {
                return 3 * sizeof(uint8_t);
            }
            default:
            {
                return 0;
            }
        }
    }
}
