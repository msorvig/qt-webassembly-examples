#include <QtCore>

#ifdef Q_OS_WASM
#include <emscripten/fetch.h>
#include <emscripten/emscripten.h>
#endif

#include <dlfcn.h>

class HelloInterface
{
public:
    virtual void hello(const QString &name) const = 0;
};
Q_DECLARE_INTERFACE(HelloInterface, "hello/1.0");

#ifdef BUILD_HOLA_PLUGIN

class HolaPlugin : public QObject, public HelloInterface
{
    Q_OBJECT
    Q_INTERFACES(HelloInterface)
    Q_PLUGIN_METADATA(IID "com.example.hello" FILE "hello.json")
    void hello(const QString &name) const
    {
        qDebug() << "Hola" <<  name;
    }
};
    
#elif BUILD_HELLO_PLUGIN

class HelloPlugin : public QObject, public HelloInterface
{
    Q_OBJECT
    Q_INTERFACES(HelloInterface)
    Q_PLUGIN_METADATA(IID "com.example.hello" FILE "hello.json")
    void hello(const QString &name) const
    {
        qDebug() << "Hello" <<  name;
    }
};

#else 

#ifdef Q_OS_WASM

// Asynchronously downloads a plugin file ("libmyplugin.so") to the in-memory file system,
// compiles and pre-loads it using emscripten_dlopen(). Calls the prepareComplete callback on
// completion, at which point the app may use QPluginLoader to load the plugin.
//
// FIXME: emscripten_dlopen() re-downloads the plugin file (though the second download will be cached).
//
enum PreparePluginResult
{
    Ok,
    FetchError,
    DlopenError,
};

void preparePlugin(const QString &sourcePath, const QString &pluginName, std::function<void (PreparePluginResult)> prepareComplete)
{
    // Add "lib" and ".so" to plugin name
    QString pluginFileName = pluginName;
    if (!pluginFileName.startsWith("lib"))
        pluginFileName.prepend("lib");
    if (!pluginFileName.endsWith(".so"))
        pluginFileName.append(".so");
    QString pluginFinalPath = sourcePath + pluginFileName;

    struct Context {
        QByteArray pluginPath;
        std::function<void (PreparePluginResult)> prepareComplete;
    };
    Context *context = new Context {
        pluginFinalPath.toUtf8(),
        std::move(prepareComplete)
    };

    // Async fetch the plugin; Qt plugin loading code expects to find the
    // plugin on the file system, and will not call dlopen() directly.
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.userData = context;
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = [](emscripten_fetch_t *fetch) {
        Context *context = static_cast<Context *>(fetch->userData);

        QByteArray data = QByteArray::fromRawData(fetch->data, fetch->numBytes);
        QFile file(context->pluginPath.constData());
        file.open(QIODevice::WriteOnly);
        file.write(data);
        file.close();
        emscripten_fetch_close(fetch);

        // Async dlopen() the plugin; Chrome refuses to sync compile wasm
        // binaries larger than ~50kb. By pre-loading it the dlopen() call
        // made by QPluginLoader will hit the already loaded library in the
        // cache and complete sucessfully on all browsers.
        emscripten_dlopen(context->pluginPath.constData(), RTLD_NOW, context,
            [](void *ctx, void *handle) {
                Q_UNUSED(handle);
                Context *context = static_cast<Context *>(ctx);
                context->prepareComplete(PreparePluginResult::Ok);
                delete context;
            },
            [](void *ctx) {
                Context *context = static_cast<Context *>(ctx);
                context->prepareComplete(PreparePluginResult::DlopenError);
                delete context;
            }
        );
    };
    attr.onerror = [](emscripten_fetch_t *fetch) {
        Context *context = static_cast<Context *>(fetch->userData);
        emscripten_fetch_close(fetch);
        context->prepareComplete(PreparePluginResult::FetchError);
        delete context;
    };
    emscripten_fetch_t *fetch = emscripten_fetch(&attr, context->pluginPath.constData());
}

// Synchronously downloads and pre-loads a plugin file
PreparePluginResult preparePluginAsyncify(const QString &sourcePath, const QString &plugin)
{
    PreparePluginResult result;
    QEventLoop loop;
    preparePlugin(sourcePath, plugin, [&loop, &result](PreparePluginResult _result){
        loop.quit();
        result = _result;
    });
    loop.exec();
    return result;
}

#endif

void loadPlugin(const QString &pluginName)
{
    QByteArray pluginPath = QFileInfo(pluginName).absoluteFilePath().toUtf8();
    void *addr = dlopen(pluginPath.constData(), RTLD_NOW);
    QPluginLoader loader(pluginName);
    QObject *plugin = loader.instance();
    if (plugin) {
        HelloInterface *helloPlugin = qobject_cast<HelloInterface *>(plugin);
        if (helloPlugin)
            helloPlugin->hello("Bob");
        else
            qDebug() << "Plugin does not export the HelloInterface interface.";
    } else {
        qDebug() << "Failed to load the plugin.";
    }
}

int main(int argc, char **argv)
{
    QCoreApplication *app = new QCoreApplication(argc, argv);
    
    qDebug() << "main";

    // TODO: figure out addLibraryPath
//    QCoreApplication::addLibraryPath("/plugins");
//    qDebug() << "library paths" << QCoreApplication::libraryPaths();
    
    // This example demonstrates how to load plugins on demand,
    // which is useful when the app does not want to pre-load all
    // plugins at startup.
    //
    // This requires changes to application code. The application must
    // make a call to preparePlugin() before loading the plugin. This
    // is an asynchronous call (with a callback) which downloads and
    // compiles the plugin.
    
    QStringList pluginNames = { "helloplugin", "holaplugin" }; // ### preparePlugin should maybe prefix "lib"
    for (QString pluginName : pluginNames) {
#ifdef Q_OS_WASM
        QString sourcePath; // empty (load relative to .html file)
        preparePlugin(sourcePath, pluginName, [pluginName](PreparePluginResult result){
            if (result == PreparePluginResult::Ok) {
                loadPlugin(pluginName);
            } else {
                 qDebug() << "plugin prepare error for plugin" << pluginName;
             }
         });
#else
         loadPlugin(pluginName);
#endif    
    }
    
    return app->exec();
}

#endif

#include "main.moc"


