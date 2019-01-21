
#include <bdn/java/JClass.h>

#include <bdn/java/Env.h>

namespace bdn
{
    namespace java
    {

        String JClass::nameInSlashNotationToSignature_(const String &nameInSlashNotation)
        {
            if (cpp20::ends_with(nameInSlashNotation, "[]"))
                return "[" +
                       nameInSlashNotationToSignature_(nameInSlashNotation.substr(0, nameInSlashNotation.length() - 2));
            else
                return "L" + nameInSlashNotation + ";";
        }

        Reference JClass::findClass_(const String &nameInSlashNotation)
        {
            Env &env = Env::get();

            // FindClass wants the name in slash notation UNLESS it is an array.
            // For arrays it wants the type signature.

            String findArg = nameInSlashNotation;
            if (cpp20::ends_with(findArg, "[]"))
                findArg = nameInSlashNotationToSignature_(nameInSlashNotation);

            jclass clazz = env.getJniEnv()->FindClass(findArg.c_str());
            env.throwAndClearExceptionFromLastJavaCall();

            return Reference::convertAndDestroyOwnedLocal((jobject)clazz);
        }

        Reference JClass::_newObject(jclass cls, jmethodID methodId, ...)
        {
            Env &env = Env::get();

            va_list argList;
            va_start(argList, methodId);

            jobject result = env.getJniEnv()->NewObjectV(cls, methodId, argList);

            va_end(argList);

            env.throwAndClearExceptionFromLastJavaCall();

            return Reference::convertAndDestroyOwnedLocal(result);
        }
    }
}
