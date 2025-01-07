#include "foreign.hpp"
#include "../ee/vm.hpp"

namespace spade
{
    void ObjForeign::call(Obj **args) {
        validateCallSite();
        Obj *result = null;
        Thread *thread = Thread::current();
        size_t count = sign.getParams().size() & 0x1ff;
        switch (count) {
            case 0:
                result = library->call<Obj *>(name);
                break;
            case 1:
                if (self == null) result = library->call<Obj *>(name, args[0]);
                else
                    result = library->call<Obj *>(name, self, args[0]);
                break;
            case 2:
                if (self == null) result = library->call<Obj *>(name, args[0], args[1]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1]);
                break;
            case 3:
                if (self == null) result = library->call<Obj *>(name, args[0], args[1], args[2]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2]);
                break;
            case 4:
                if (self == null) result = library->call<Obj *>(name, args[0], args[1], args[2], args[3]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3]);
                break;
            case 5:
                if (self == null) result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4]);
                break;
            case 6:
                if (self == null) result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5]);
                break;
            case 7:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
                break;
            case 8:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7]);
                break;
            case 9:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8]);
                break;
            case 10:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9]);
                break;
            case 11:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10]);
                break;
            case 12:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11]);
                break;
            case 13:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12]);
                break;
            case 14:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13]);
                break;
            case 15:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14]);
                break;
            case 16:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15]);
                break;
            case 17:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16]);
                break;
            case 18:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17]);
                break;
            case 19:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18]);
                break;
            case 20:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19]);
                break;
            case 21:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20]);
                break;
            case 22:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20], args[21]);
                break;
            case 23:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21], args[22]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20], args[21],
                                                  args[22]);
                break;
            case 24:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21], args[22],
                                                  args[23]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20], args[21],
                                                  args[22], args[23]);
                break;
            case 25:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21], args[22],
                                                  args[23], args[24]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20], args[21],
                                                  args[22], args[23], args[24]);
                break;
            case 26:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21], args[22],
                                                  args[23], args[24], args[25]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20], args[21],
                                                  args[22], args[23], args[24], args[25]);
                break;
            case 27:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21], args[22],
                                                  args[23], args[24], args[25], args[26]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20], args[21],
                                                  args[22], args[23], args[24], args[25], args[26]);
                break;
            case 28:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21], args[22],
                                                  args[23], args[24], args[25], args[26], args[27]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20], args[21],
                                                  args[22], args[23], args[24], args[25], args[26], args[27]);
                break;
            case 29:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21], args[22],
                                                  args[23], args[24], args[25], args[26], args[27], args[28]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20], args[21],
                                                  args[22], args[23], args[24], args[25], args[26], args[27], args[28]);
                break;
            case 30:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21], args[22],
                                                  args[23], args[24], args[25], args[26], args[27], args[28], args[29]);
                else
                    result =
                            library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                 args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                 args[15], args[16], args[17], args[18], args[19], args[20], args[21], args[22],
                                                 args[23], args[24], args[25], args[26], args[27], args[28], args[29]);
                break;
            case 31:
                if (self == null)
                    result =
                            library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                 args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                 args[16], args[17], args[18], args[19], args[20], args[21], args[22], args[23],
                                                 args[24], args[25], args[26], args[27], args[28], args[29], args[30]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20], args[21],
                                                  args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                                                  args[29], args[30]);
                break;
            case 32:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21], args[22],
                                                  args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                                                  args[30], args[31]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20], args[21],
                                                  args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                                                  args[29], args[30], args[31]);
                break;
            case 33:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21], args[22],
                                                  args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                                                  args[30], args[31], args[32]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20], args[21],
                                                  args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                                                  args[29], args[30], args[31], args[32]);
                break;
            case 34:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21], args[22],
                                                  args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                                                  args[30], args[31], args[32], args[33]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20], args[21],
                                                  args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                                                  args[29], args[30], args[31], args[32], args[33]);
                break;
            case 35:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21], args[22],
                                                  args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                                                  args[30], args[31], args[32], args[33], args[34]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20], args[21],
                                                  args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                                                  args[29], args[30], args[31], args[32], args[33], args[34]);
                break;
            case 36:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21], args[22],
                                                  args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                                                  args[30], args[31], args[32], args[33], args[34], args[35]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20], args[21],
                                                  args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                                                  args[29], args[30], args[31], args[32], args[33], args[34], args[35]);
                break;
            case 37:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21], args[22],
                                                  args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                                                  args[30], args[31], args[32], args[33], args[34], args[35], args[36]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36]);
                break;
            case 38:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37]);
                break;
            case 39:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38]);
                break;
            case 40:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20], args[21],
                                                  args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                                                  args[29], args[30], args[31], args[32], args[33], args[34], args[35],
                                                  args[36], args[37], args[38], args[39]);
                break;
            case 41:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21], args[22],
                                                  args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                                                  args[30], args[31], args[32], args[33], args[34], args[35], args[36],
                                                  args[37], args[38], args[39], args[40]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20], args[21],
                                                  args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                                                  args[29], args[30], args[31], args[32], args[33], args[34], args[35],
                                                  args[36], args[37], args[38], args[39], args[40]);
                break;
            case 42:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21], args[22],
                                                  args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                                                  args[30], args[31], args[32], args[33], args[34], args[35], args[36],
                                                  args[37], args[38], args[39], args[40], args[41]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20], args[21],
                                                  args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                                                  args[29], args[30], args[31], args[32], args[33], args[34], args[35],
                                                  args[36], args[37], args[38], args[39], args[40], args[41]);
                break;
            case 43:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21], args[22],
                                                  args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                                                  args[30], args[31], args[32], args[33], args[34], args[35], args[36],
                                                  args[37], args[38], args[39], args[40], args[41], args[42]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20], args[21],
                                                  args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                                                  args[29], args[30], args[31], args[32], args[33], args[34], args[35],
                                                  args[36], args[37], args[38], args[39], args[40], args[41], args[42]);
                break;
            case 44:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21], args[22],
                                                  args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                                                  args[30], args[31], args[32], args[33], args[34], args[35], args[36],
                                                  args[37], args[38], args[39], args[40], args[41], args[42], args[43]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43]);
                break;
            case 45:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44]);
                break;
            case 46:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45]);
                break;
            case 47:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46]);
                break;
            case 48:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47]);
                break;
            case 49:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48]);
                break;
            case 50:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49]);
                else
                    result = library->call<Obj *>(name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6],
                                                  args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14],
                                                  args[15], args[16], args[17], args[18], args[19], args[20], args[21],
                                                  args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                                                  args[29], args[30], args[31], args[32], args[33], args[34], args[35],
                                                  args[36], args[37], args[38], args[39], args[40], args[41], args[42],
                                                  args[43], args[44], args[45], args[46], args[47], args[48], args[49]);
                break;
            case 51:
                if (self == null)
                    result = library->call<Obj *>(name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7],
                                                  args[8], args[9], args[10], args[11], args[12], args[13], args[14], args[15],
                                                  args[16], args[17], args[18], args[19], args[20], args[21], args[22],
                                                  args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                                                  args[30], args[31], args[32], args[33], args[34], args[35], args[36],
                                                  args[37], args[38], args[39], args[40], args[41], args[42], args[43],
                                                  args[44], args[45], args[46], args[47], args[48], args[49], args[50]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50]);
                break;
            case 52:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51]);
                break;
            case 53:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52]);
                break;
            case 54:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53]);
                break;
            case 55:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54]);
                break;
            case 56:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55]);
                break;
            case 57:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56]);
                break;
            case 58:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57]);
                break;
            case 59:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58]);
                break;
            case 60:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59]);
                break;
            case 61:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60]);
                break;
            case 62:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61]);
                break;
            case 63:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62]);
                break;
            case 64:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63]);
                break;
            case 65:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64]);
                break;
            case 66:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65]);
                break;
            case 67:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66]);
                break;
            case 68:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67]);
                break;
            case 69:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68]);
                break;
            case 70:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69]);
                break;
            case 71:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70]);
                break;
            case 72:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71]);
                break;
            case 73:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72]);
                break;
            case 74:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73]);
                break;
            case 75:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74]);
                break;
            case 76:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75]);
                break;
            case 77:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76]);
                break;
            case 78:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77]);
                break;
            case 79:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78]);
                break;
            case 80:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79]);
                break;
            case 81:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80]);
                break;
            case 82:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81]);
                break;
            case 83:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82]);
                break;
            case 84:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83]);
                break;
            case 85:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84]);
                break;
            case 86:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85]);
                break;
            case 87:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86]);
                break;
            case 88:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87]);
                break;
            case 89:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88]);
                break;
            case 90:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89]);
                break;
            case 91:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90]);
                break;
            case 92:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91]);
                break;
            case 93:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92]);
                break;
            case 94:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93]);
                break;
            case 95:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94]);
                break;
            case 96:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95]);
                break;
            case 97:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96]);
                break;
            case 98:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97]);
                break;
            case 99:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98]);
                break;
            case 100:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99]);
                break;
            case 101:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100]);
                break;
            case 102:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101]);
                break;
            case 103:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102]);
                break;
            case 104:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103]);
                break;
            case 105:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104]);
                break;
            case 106:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105]);
                break;
            case 107:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106]);
                break;
            case 108:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107]);
                break;
            case 109:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108]);
                break;
            case 110:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109]);
                break;
            case 111:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110]);
                break;
            case 112:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111]);
                break;
            case 113:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112]);
                break;
            case 114:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113]);
                break;
            case 115:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114]);
                break;
            case 116:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115]);
                break;
            case 117:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116]);
                break;
            case 118:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117]);
                break;
            case 119:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118]);
                break;
            case 120:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119]);
                break;
            case 121:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120]);
                break;
            case 122:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121]);
                break;
            case 123:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122]);
                break;
            case 124:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123]);
                break;
            case 125:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124]);
                break;
            case 126:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125]);
                break;
            case 127:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126]);
                break;
            case 128:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127]);
                break;
            case 129:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128]);
                break;
            case 130:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129]);
                break;
            case 131:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130]);
                break;
            case 132:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131]);
                break;
            case 133:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132]);
                break;
            case 134:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133]);
                break;
            case 135:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134]);
                break;
            case 136:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135]);
                break;
            case 137:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136]);
                break;
            case 138:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137]);
                break;
            case 139:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138]);
                break;
            case 140:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139]);
                break;
            case 141:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140]);
                break;
            case 142:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141]);
                break;
            case 143:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142]);
                break;
            case 144:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143]);
                break;
            case 145:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144]);
                break;
            case 146:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145]);
                break;
            case 147:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146]);
                break;
            case 148:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147]);
                break;
            case 149:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148]);
                break;
            case 150:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149]);
                break;
            case 151:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150]);
                break;
            case 152:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151]);
                break;
            case 153:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152]);
                break;
            case 154:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153]);
                break;
            case 155:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154]);
                break;
            case 156:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155]);
                break;
            case 157:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156]);
                break;
            case 158:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157]);
                break;
            case 159:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158]);
                break;
            case 160:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159]);
                break;
            case 161:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160]);
                break;
            case 162:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161]);
                break;
            case 163:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162]);
                break;
            case 164:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163]);
                break;
            case 165:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164]);
                break;
            case 166:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165]);
                break;
            case 167:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166]);
                break;
            case 168:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167]);
                break;
            case 169:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168]);
                break;
            case 170:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169]);
                break;
            case 171:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170]);
                break;
            case 172:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171]);
                break;
            case 173:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172]);
                break;
            case 174:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173]);
                break;
            case 175:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174]);
                break;
            case 176:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175]);
                break;
            case 177:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176]);
                break;
            case 178:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177]);
                break;
            case 179:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178]);
                break;
            case 180:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179]);
                break;
            case 181:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180]);
                break;
            case 182:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181]);
                break;
            case 183:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182]);
                break;
            case 184:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183]);
                break;
            case 185:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184]);
                break;
            case 186:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185]);
                break;
            case 187:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186]);
                break;
            case 188:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187]);
                break;
            case 189:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188]);
                break;
            case 190:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189]);
                break;
            case 191:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190]);
                break;
            case 192:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191]);
                break;
            case 193:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192]);
                break;
            case 194:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193]);
                break;
            case 195:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194]);
                break;
            case 196:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195]);
                break;
            case 197:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196]);
                break;
            case 198:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197]);
                break;
            case 199:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198]);
                break;
            case 200:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199]);
                break;
            case 201:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200]);
                break;
            case 202:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201]);
                break;
            case 203:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202]);
                break;
            case 204:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203]);
                break;
            case 205:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204]);
                break;
            case 206:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205]);
                break;
            case 207:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206]);
                break;
            case 208:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207]);
                break;
            case 209:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208]);
                break;
            case 210:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209]);
                break;
            case 211:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210]);
                break;
            case 212:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211]);
                break;
            case 213:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212]);
                break;
            case 214:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213]);
                break;
            case 215:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214]);
                break;
            case 216:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215]);
                break;
            case 217:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216]);
                break;
            case 218:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217]);
                break;
            case 219:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218]);
                break;
            case 220:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219]);
                break;
            case 221:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220]);
                break;
            case 222:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221]);
                break;
            case 223:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222]);
                break;
            case 224:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223]);
                break;
            case 225:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224]);
                break;
            case 226:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225]);
                break;
            case 227:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226]);
                break;
            case 228:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227]);
                break;
            case 229:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228]);
                break;
            case 230:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229]);
                break;
            case 231:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230]);
                break;
            case 232:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231]);
                break;
            case 233:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232]);
                break;
            case 234:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233]);
                break;
            case 235:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234]);
                break;
            case 236:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235]);
                break;
            case 237:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236]);
                break;
            case 238:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236], args[237]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236], args[237]);
                break;
            case 239:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236], args[237], args[238]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236], args[237], args[238]);
                break;
            case 240:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236], args[237], args[238], args[239]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236], args[237], args[238], args[239]);
                break;
            case 241:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236], args[237], args[238], args[239], args[240]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236], args[237], args[238], args[239], args[240]);
                break;
            case 242:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236], args[237], args[238], args[239], args[240], args[241]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236], args[237], args[238], args[239], args[240], args[241]);
                break;
            case 243:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242]);
                break;
            case 244:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242], args[243]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242],
                            args[243]);
                break;
            case 245:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242], args[243],
                            args[244]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242],
                            args[243], args[244]);
                break;
            case 246:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242], args[243],
                            args[244], args[245]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242],
                            args[243], args[244], args[245]);
                break;
            case 247:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242], args[243],
                            args[244], args[245], args[246]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242],
                            args[243], args[244], args[245], args[246]);
                break;
            case 248:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242], args[243],
                            args[244], args[245], args[246], args[247]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242],
                            args[243], args[244], args[245], args[246], args[247]);
                break;
            case 249:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242], args[243],
                            args[244], args[245], args[246], args[247], args[248]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242],
                            args[243], args[244], args[245], args[246], args[247], args[248]);
                break;
            case 250:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242], args[243],
                            args[244], args[245], args[246], args[247], args[248], args[249]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242],
                            args[243], args[244], args[245], args[246], args[247], args[248], args[249]);
                break;
            case 251:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242], args[243],
                            args[244], args[245], args[246], args[247], args[248], args[249], args[250]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242],
                            args[243], args[244], args[245], args[246], args[247], args[248], args[249], args[250]);
                break;
            case 252:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242], args[243],
                            args[244], args[245], args[246], args[247], args[248], args[249], args[250], args[251]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242],
                            args[243], args[244], args[245], args[246], args[247], args[248], args[249], args[250], args[251]);
                break;
            case 253:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242], args[243],
                            args[244], args[245], args[246], args[247], args[248], args[249], args[250], args[251], args[252]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242],
                            args[243], args[244], args[245], args[246], args[247], args[248], args[249], args[250], args[251],
                            args[252]);
                break;
            case 254:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242], args[243],
                            args[244], args[245], args[246], args[247], args[248], args[249], args[250], args[251], args[252],
                            args[253]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242],
                            args[243], args[244], args[245], args[246], args[247], args[248], args[249], args[250], args[251],
                            args[252], args[253]);
                break;
            case 255:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242], args[243],
                            args[244], args[245], args[246], args[247], args[248], args[249], args[250], args[251], args[252],
                            args[253], args[254]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242],
                            args[243], args[244], args[245], args[246], args[247], args[248], args[249], args[250], args[251],
                            args[252], args[253], args[254]);
                break;
            case 256:
                if (self == null)
                    result = library->call<Obj *>(
                            name, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
                            args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
                            args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
                            args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
                            args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
                            args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
                            args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
                            args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
                            args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
                            args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99],
                            args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107], args[108],
                            args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116], args[117],
                            args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125], args[126],
                            args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134], args[135],
                            args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143], args[144],
                            args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152], args[153],
                            args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161], args[162],
                            args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170], args[171],
                            args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179], args[180],
                            args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188], args[189],
                            args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197], args[198],
                            args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206], args[207],
                            args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215], args[216],
                            args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224], args[225],
                            args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233], args[234],
                            args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242], args[243],
                            args[244], args[245], args[246], args[247], args[248], args[249], args[250], args[251], args[252],
                            args[253], args[254], args[255]);
                else
                    result = library->call<Obj *>(
                            name, self, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8],
                            args[9], args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18],
                            args[19], args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28],
                            args[29], args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38],
                            args[39], args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48],
                            args[49], args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58],
                            args[59], args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68],
                            args[69], args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78],
                            args[79], args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88],
                            args[89], args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98],
                            args[99], args[100], args[101], args[102], args[103], args[104], args[105], args[106], args[107],
                            args[108], args[109], args[110], args[111], args[112], args[113], args[114], args[115], args[116],
                            args[117], args[118], args[119], args[120], args[121], args[122], args[123], args[124], args[125],
                            args[126], args[127], args[128], args[129], args[130], args[131], args[132], args[133], args[134],
                            args[135], args[136], args[137], args[138], args[139], args[140], args[141], args[142], args[143],
                            args[144], args[145], args[146], args[147], args[148], args[149], args[150], args[151], args[152],
                            args[153], args[154], args[155], args[156], args[157], args[158], args[159], args[160], args[161],
                            args[162], args[163], args[164], args[165], args[166], args[167], args[168], args[169], args[170],
                            args[171], args[172], args[173], args[174], args[175], args[176], args[177], args[178], args[179],
                            args[180], args[181], args[182], args[183], args[184], args[185], args[186], args[187], args[188],
                            args[189], args[190], args[191], args[192], args[193], args[194], args[195], args[196], args[197],
                            args[198], args[199], args[200], args[201], args[202], args[203], args[204], args[205], args[206],
                            args[207], args[208], args[209], args[210], args[211], args[212], args[213], args[214], args[215],
                            args[216], args[217], args[218], args[219], args[220], args[221], args[222], args[223], args[224],
                            args[225], args[226], args[227], args[228], args[229], args[230], args[231], args[232], args[233],
                            args[234], args[235], args[236], args[237], args[238], args[239], args[240], args[241], args[242],
                            args[243], args[244], args[245], args[246], args[247], args[248], args[249], args[250], args[251],
                            args[252], args[253], args[254], args[255]);
                break;
            default:
                result = ObjNull::value();
                break;
        }

        thread->getState()->push(result);
    }
}
