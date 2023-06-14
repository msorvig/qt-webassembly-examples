#include <QtWidgets>
#include <emscripten/val.h>

uint64_t M = 1024 * 1024;
uint64_t operator ""_m(unsigned long long val) { return val * M; };

struct HeapAllocation
{
    void *mem;
    uint64_t size;
};
uint32_t heapAllocSize = 256_m;
uint64_t totalAllocSize = 0;
QVector<HeapAllocation> heapMemory;


struct JsAllocation
{
    emscripten::val mem;
    uint64_t size;
};

uint32_t jsAllocSize = 256_m;
uint64_t totalJsAllocSize = 0;
QVector<JsAllocation> jsMemory;

class MemoryAllocatorWidget : public QWidget
{
public:
    MemoryAllocatorWidget() {
        
        heapMemory.reserve(128);
        jsMemory.reserve(128);

        QHBoxLayout *line1 = new QHBoxLayout();
        QPushButton *allocHeap = new QPushButton(QString("Allocate %1 MB Emscripten Heap Memory").arg(heapAllocSize / M));
        QLabel *heapSize = new QLabel();
        
        auto updateHeapSize = [heapSize](uint64_t delta){
            totalAllocSize += delta;
            heapSize->setText(QString("Emscripten Heap: %1 MB").arg(totalAllocSize / M));
        };
        updateHeapSize(0);

        QObject::connect(allocHeap, &QPushButton::clicked, [updateHeapSize](){
            HeapAllocation alloc {
                malloc(heapAllocSize),
                heapAllocSize,
            };
            if (alloc.mem == 0) {
                qDebug() << "Unable to allocate more heap memory";
                return;
            }
            memset(alloc.mem, 1, alloc.size); // touch memory to avoid overcommit
            updateHeapSize(alloc.size);
            heapMemory.push_back(std::move(alloc));
        });
        
        QPushButton *freeHeap = new QPushButton("Free Memory");
        QObject::connect(freeHeap, &QPushButton::clicked, [updateHeapSize](){
            if (heapMemory.isEmpty())
                return;
            auto alloc = heapMemory.takeLast();
            free(alloc.mem);
            updateHeapSize(-alloc.size);
        });
        
        line1->addWidget(allocHeap);
        line1->addWidget(freeHeap);
        line1->addWidget(heapSize);
        line1->addStretch();
        
        QHBoxLayout *line2 = new QHBoxLayout();
        QPushButton *allocJs = new QPushButton(QString("Allocate %1 MB JavaScript Heap Memory").arg(jsAllocSize / M));
        QLabel *jsSize = new QLabel();

        auto updateJsSize = [jsSize](uint64_t delta){
            totalJsAllocSize += delta;
            jsSize->setText(QString("JavaScript Heap: %1 MB").arg(totalJsAllocSize / M));
        };
        updateJsSize(0);

        QObject::connect(allocJs, &QPushButton::clicked, [updateJsSize](){
            using emscripten::val;
            const val buffer = val::global("SharedArrayBuffer").new_(jsAllocSize);
            JsAllocation alloc {
                val::global("Uint8Array").new_(buffer),
                jsAllocSize,
            };
            alloc.mem.call<void>("fill", 1);
            updateJsSize(alloc.size);
            jsMemory.push_back(std::move(alloc));
        });
        
        QPushButton *freeJs = new QPushButton("Free Memory");
        QObject::connect(freeJs, &QPushButton::clicked, [updateJsSize](){
            if (jsMemory.isEmpty())
                return;
            auto alloc = jsMemory.takeLast();
            alloc.mem = emscripten::val::undefined(); // drop reference
            updateJsSize(-alloc.size);
        });

        line2->addWidget(allocJs);
        line2->addWidget(freeJs);
        line2->addWidget(jsSize);
        line2->addStretch();

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addLayout(line1);
        layout->addLayout(line2);
        layout->addStretch();
        
        setLayout(layout);
    }
};

int main(int argc, char **argv)
{
    QApplication *app = new QApplication(argc, argv);
        
    MemoryAllocatorWidget *maw = new MemoryAllocatorWidget();
    maw->show();
}