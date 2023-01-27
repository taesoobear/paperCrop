javac -d bin -cp bin:lib/iText.jar:lib/iText-xtra.jar *.java
java -cp bin:lib/iText.jar:lib/iText-xtra.jar PdfCrop test.pdf crop.pdf test.xml
