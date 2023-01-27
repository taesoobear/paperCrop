/*
 * This class is written by Taesoo Kwon based on the examples of the itextpdf library. 
 * For more info, go to: http://itextpdf.com/examples/
 */

import java.io.*;
import java.util.Scanner;
import java.util.ArrayList;
import java.util.StringTokenizer;
import com.itextpdf.text.Document;
import com.itextpdf.text.DocumentException;
import com.itextpdf.text.Element;
import com.itextpdf.text.Font;
import com.itextpdf.text.Image;
import com.itextpdf.text.PageSize;
import com.itextpdf.text.Paragraph;
import com.itextpdf.text.Font.FontFamily;
import com.itextpdf.text.pdf.*;
//import com.itextpdf.text.pdf.PdfContentByte;
//import com.itextpdf.text.pdf.PdfGState;
//import com.itextpdf.text.pdf.PdfImportedPage;
//import com.itextpdf.text.pdf.PdfReader;
//import com.itextpdf.text.pdf.PdfWriter;
//import com.itextpdf.text.pdf.PdfCopy;
import com.itextpdf.text.Rectangle;
import static java.lang.System.out;
class Rect {
	public float left, top, right, bottom;
	Rect(float l, float t, float r, float b){
		left=l; top=t; right=r; bottom=b;
	}
	Rect(float r, float b){
		this(0,0,r,b);
	}
	public String toString(){
		return ""+left+","+top+","+right+","+bottom+"\n";
	}
	float width(){ return right-left;}
	float height(){ return bottom-top;}
	void scale(float s)
	{
		left*=s;
		top*=s;
		right*=s;
		bottom*=s;
	}
	Rect flipY()
	{
		return new Rect(left, 1-bottom, right, 1-top);
	}
	void translateY(float y)
	{
		top+=y;
		bottom+=y;
	}
}
class TRect extends Rect {
	public float scaleFactor;
	TRect(float r, float b){
		super(r,b);
		scaleFactor=1;
	}
	TRect(float l, float t, float r, float b, float s){
		super(l,t,r,b);
		scaleFactor=s;
	}
	void scale(float s) { super.scale(s); scaleFactor*=s;}
}
class PageInfo {
	public ArrayList<Rect> rects=new ArrayList<Rect>();
	public ArrayList<TRect> targetRects=new ArrayList<TRect>();
	public int pdfPage;
	PageInfo() { pdfPage=-1;}
}
public class PdfCrop {

	public static byte[] readPDF(String filename, String rangeOfPagesToReplicate) throws RuntimeException 
	{
		String range[] = rangeOfPagesToReplicate.split("-");
		int startPage = Integer.parseInt(range[0]);
		int endPage = Integer.parseInt(range[1]);

		try {
			PdfReader reader = new PdfReader(filename);
			return extractPages(reader, startPage, endPage);
		} catch (Exception e) {
			throw new RuntimeException(e);
		} finally {
		}
	}

	public static void writePDF(byte b[], String filename) throws RuntimeException
	{
		PdfStamper stamper = null;
		Document document = null;
		PdfCopy copy = null;

		try {
			PdfReader reader = new PdfReader(new ByteArrayInputStream(b));
			int totalPages = reader.getNumberOfPages();


			document = new Document(reader.getPageSizeWithRotation(1));
			copy = new PdfCopy(document, new FileOutputStream(filename));
			document.open();
			int pages=reader.getNumberOfPages();
			PdfImportedPage page;
			for (int j = 0; j < pages;) {
				++j;
				page = copy.getImportedPage(reader, j);
				copy.addPage(page);
			}
			PRAcroForm form = reader.getAcroForm();
			if (form != null) {
				copy.copyAcroForm(reader);
			}
		} catch (Exception e) {
			throw new RuntimeException(e);
		} finally {
			document.close();
			copy.close();
		}
	}

	public static byte[] extractPages(PdfReader reader, int startPage, int endPage) throws RuntimeException 
	{
		ByteArrayOutputStream baos = null;
		PdfStamper stamper = null;
		Document document = null;
		PdfCopy copy = null;

		baos = new ByteArrayOutputStream();
		try {
			int totalPages = reader.getNumberOfPages();

			if (endPage>totalPages)
				endPage=totalPages;

			if(startPage < 1 || endPage > totalPages){
				throw new RuntimeException("invalid range");
			}

			document = new Document(reader.getPageSizeWithRotation(1));
			// For testing purpose
			/*
			 * copy = new PdfCopy(document, new
			 FileOutputStream(
			 * [ ] "c:\\Ramdas\\iText\\outPut.pdf"));
			 */
			copy = new PdfCopy(document, baos);
			document.open();
			int pages=reader.getNumberOfPages();
			PdfImportedPage page;
			for (int j = startPage; j<=endPage; j++){
				page = copy.getImportedPage(reader, j);
				copy.addPage(page);
			}
			PRAcroForm form = reader.getAcroForm();
			if (form != null) {
				copy.copyAcroForm(reader);
			}
		} catch (Exception e) {
			throw new RuntimeException(e);
		} finally {
			document.close();
			copy.close();
		}

		return baos.toByteArray();
	}
	/*public static ByteArrayOutputStream replicatePages(byte b[], String rangeOfPagesToReplicate, int howManyTimes) throws RuntimeException 
	{
		String range[] = rangeOfPagesToReplicate.split("-");
		int startPage = Integer.parseInt(range[0]);
		int endPage = Integer.parseInt(range[1]);
		return replicatePages(b, startPage, endPage,howManyTimes);
	}*/

	public static ArrayList<PdfReader> replicatePages(byte b[], int howManyTimes)
	{
		ArrayList<PdfReader> readerList = new ArrayList<PdfReader>(10);
		try {
			PdfStamper stamper = null;
			ByteArrayOutputStream baos = new ByteArrayOutputStream();
			PdfReader reader = new PdfReader(new ByteArrayInputStream(b));
			int totalPages = reader.getNumberOfPages();
			readerList.add(reader);// first part - static
			/*
			 * Following loop will duplicate the pages and
			 also rename the
			 * fields
			 */
			for (int j = 0; j < howManyTimes; j++) {
				baos.reset();
				reader = new PdfReader(new ByteArrayInputStream(b));
				stamper = new PdfStamper(reader, baos);
				AcroFields fields = stamper.getAcroFields();

				if (fields != null && !fields.getFields().isEmpty()) {
					String a[] = (String[])
						fields.getFields().keySet() .toArray(new String[1]);
					for (int i = 0; i < a.length; i++) {
						fields.renameField(a[i], a[i] + "_" + (j + 1));
					}
				}
				stamper.close();
				readerList.add(reader);
			}
		} catch (Exception e) {
			throw new RuntimeException(e);
		} 
		return readerList;
	}

    /**
     * Main method.
     * @param args no arguments needed
     * @throws DocumentException 
     * @throws IOException
     */
    public static void main(String[] args)
        throws IOException, DocumentException {

		//byte [] o=readPDF(SOURCE,"1-4");
		//writePDF(o, "results/layers_copy.pdf");
		//writePDF(replicatePages(o,"1-2",4).toByteArray(), "results/layers_copy.pdf");

		if (args.length<3){
		   System.out.println("Usage: java -jar PdfCrop.jar test.pdf test_1.pdf test.xml");
	   	   return;
		}
		System.out.println(args[0]);
		System.out.println(args[1]);
		System.out.println(args[2]);
		String SRC=args[0];
		String TGT=args[1];
		String xml=args[2];
		ArrayList<PageInfo> pageInfo=new ArrayList<PageInfo>();
		{
			pageInfo.add(new PageInfo());
			PageInfo lastPage=pageInfo.get(pageInfo.size()-1);
			Scanner scanner = new Scanner(new FileReader(xml));
			try {
				//first use a Scanner to get each line
				while ( scanner.hasNextLine() ){
					String line=scanner.nextLine(); 
					StringTokenizer t=new StringTokenizer(line, "= ,;{}\n\t\r") ;
					while(t.hasMoreTokens()){
						String token=t.nextToken();
						if (token.equals("pdfPage")) 
						{
							String tkn=t.nextToken();
							lastPage.pdfPage=Integer.valueOf(tkn);
							pageInfo.add(new PageInfo());
							lastPage=pageInfo.get(pageInfo.size()-1);
						}
						else
						{
							float l=Float.valueOf(token);
							float top=Float.valueOf(t.nextToken());
							float r=Float.valueOf(t.nextToken());
							float b=Float.valueOf(t.nextToken());
							lastPage.rects.add(new Rect(l,top,r,b));
							l=Float.valueOf(t.nextToken());
							top=Float.valueOf(t.nextToken());
							r=Float.valueOf(t.nextToken());
							b=Float.valueOf(t.nextToken());
							float s=Float.valueOf(t.nextToken());
							lastPage.targetRects.add(new TRect(l,top,r,b,s));
							//out.println(l+","+top+","+r+","+b+","+s);
						}
					}
				}
			}
			finally {
				//ensure the underlying stream is always closed
				//this only has any effect if the item passed to the Scanner
				//constructor implements Closeable (which it does in this case).
				scanner.close();
			}
		}
		{
			PdfReader reader = new PdfReader(SRC);
		 	ArrayList<Rect> rects=pageInfo.get(0).rects;
			ArrayList<TRect> targetRects=pageInfo.get(0).targetRects;
			float totalHeight=targetRects.get(targetRects.size()-1).bottom;
			//out.println(totalHeight);
			int numRects=rects.size();
			// step 1
			Rectangle ss=reader.getPageSize(1);
			Document document = new Document(new Rectangle(0,0,ss.getWidth(),(float)(ss.getHeight()*totalHeight))); //PageSize.A5.rotate());
			// step 2
			PdfWriter writer
				= PdfWriter.getInstance(document, new FileOutputStream(TGT));
			// step 3
			document.open();
			// step 4
			PdfContentByte canvas = writer.getDirectContent();
			PdfImportedPage page;
			BaseFont bf = BaseFont.createFont(BaseFont.ZAPFDINGBATS, "", BaseFont.EMBEDDED);

			for (int iipage=0; iipage<pageInfo.size()-1; iipage++) {
				int ipage=pageInfo.get(iipage).pdfPage;
				System.out.println("processing page "+ipage);
				rects=pageInfo.get(iipage).rects;
				targetRects=pageInfo.get(iipage).targetRects;
				totalHeight=targetRects.get(targetRects.size()-1).bottom;
				numRects=rects.size();

				ArrayList<PdfReader> page1r=replicatePages(extractPages(reader,ipage,ipage), 1);//numRects);
				ss=reader.getPageSize(ipage);
				if (iipage!=0){
					document.setPageSize(new Rectangle(0,0,ss.getWidth(),(float)(ss.getHeight()*totalHeight)));
					document.newPage();
				}
				PdfImportedPage p= writer.getImportedPage(page1r.get(0), 1);
				for(int i=0; i<numRects; i++) {
					//for(int i=1; i<2; i++){
					//Rectangle cropBox=reader.getCropBox(i);
					//System.out.printf("%f %f %f %f\n",cropBox.getLeft(), cropBox.getTop(), cropBox.getRight(), cropBox.getBottom());

					//PdfImportedPage p= writer.getImportedPage(reader, i);

					Rect ri=rects.get(i).flipY();
					ri.left*=ss.getWidth();
					ri.top*=ss.getHeight();
					ri.right*=ss.getWidth();
					ri.bottom*=ss.getHeight();
					PdfTemplate nt=canvas.createTemplate(ss.getWidth(),ss.getHeight());
					nt.addTemplate(p, 1f,0,0,1f,0,0);
					nt.setBoundingBox(new Rectangle(ri.left, ri.top, ri.right, ri.bottom));
					float s=targetRects.get(i).scaleFactor;
					//System.out.println("scale"+s);

					//canvas.addTemplate(p, s, 0, 0, s, -ri.left*s ,-ri.bottom*s+ss.getHeight()*(totalHeight-targetRects.get(i).top));
					canvas.addTemplate(nt, s, 0, 0, s, -ri.left*s ,-ri.bottom*s+ss.getHeight()*(totalHeight-targetRects.get(i).top));
					}
				}
			// step 5
			document.close();
		}

    }
    
    /**
     * Draws a rectangle
     * @param content the direct content layer
     * @param width the width of the rectangle
     * @param height the height of the rectangle
     */
    public static void drawRectangle(PdfContentByte content, float width, float height) {
        content.saveState();
        PdfGState state = new PdfGState();
        state.setFillOpacity(0.6f);
        content.setGState(state);
        content.setRGBColorFill(0xFF, 0xFF, 0xFF);
        content.setLineWidth(3);
        content.rectangle(0, 0, width, height);
        content.fillStroke();
        content.restoreState();
    }
}
