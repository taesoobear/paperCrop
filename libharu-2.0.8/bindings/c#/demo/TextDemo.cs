/*
 * << Haru Free PDF Library 2.0.5 >> -- TextDemo.cs
 *
 * Copyright (c) 1999-2006 Takeshi Kanno <takeshi_kanno@est.hi-ho.ne.jp>
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.
 * It is provided "as is" without express or implied warranty.
 *
 */

using System;
using System.IO;
using HPdf;


public class TextDemo {
    private static void ShowStripePattern (HPdfPage page, float x, float y) {
        int iy = 0;

        while (iy < 50) {
            page.SetRGBStroke(0.0f, 0.0f, 0.5f);
            page.SetLineWidth(1);
            page.MoveTo(x, y + iy);
            page.LineTo(x + page.TextWidth("ABCabc123"),
                    y + iy);
            page.Stroke();
            iy += 3;
        }

        page.SetLineWidth(2.5f);
    }

    private static void ShowDescription (HPdfPage page, float x, float y,
            string text) {
        float fsize = page.GetCurrentFontSize();
        HPdfFont font = page.GetCurrentFont();
        HPdfRGBColor c = page.GetRGBFill();

        page.BeginText();
        page.SetRGBFill(0, 0, 0);
        page.SetTextRenderingMode(HPdfTextRenderingMode.HPDF_FILL);
        page.SetFontAndSize(font, 10);
        page.TextOut(x, y - 12, text);
        page.EndText();

        page.SetFontAndSize(font, fsize);
        page.SetRGBFill(c.r, c.g, c.b);
    }



    public static void Main() {
        const string page_title = "Text Demo";
        const string samp_text = "abcdefgABCDEFG123!#$%&+-@?";
        const string samp_text2 = "The quick brown fox jumps over the lazy dog.";
        Console.WriteLine("libhpdf-" + HPdfDoc.HPdfGetVersion());

        try {
            HPdfDoc pdf = new HPdfDoc();

            pdf.SetCompressionMode(HPdfDoc.HPDF_COMP_ALL);

            /* create default-font */
            HPdfFont font = pdf.GetFont("Helvetica", null);

            /* add a new page object. */
            HPdfPage page = pdf.AddPage();

            /*print the title of the page (with positioning center). */
            page.SetFontAndSize(font, 24);
            float tw = page.TextWidth(page_title);
            page.BeginText();
            page.TextOut((page.GetWidth() - tw) / 2,
                        page.GetHeight () - 50, page_title);
            page.EndText();

            page.BeginText();
            page.MoveTextPos(60, page.GetHeight() - 60);

            /*
             * font size
             */
            float fsize = 8;
            while (fsize < 60) {
                /* set style and size of font. */
                page.SetFontAndSize(font, fsize);

                /* set the position of the text. */
                page.MoveTextPos(0, -5 - fsize);

                /* measure the number of characters which included in the page. */
                uint tlen = page.MeasureText(samp_text,
                                page.GetWidth() - 120, false);

                page.ShowText(samp_text.Substring(0, (int)tlen));

                /* print the description. */
                page.MoveTextPos(0, -10);
                page.SetFontAndSize(font, 8);
                page.ShowText("Fontsize=" + fsize);

                fsize *= 1.5f;
            }

            /*
             * font color
             */
            page.SetFontAndSize(font, 8);
            page.MoveTextPos(0, -30);
            page.ShowText("Font color");

            page.SetFontAndSize(font, 18);
            page.MoveTextPos(0, -20);
            int len = samp_text.Length;

            for (int i = 0; i < len; i++) {
                string buf;

                float r = (float)i / (float)len;
                float g = 1 - ((float)i / (float)len);
                buf = samp_text.Substring(i, 1);

                page.SetRGBFill(r, g, 0);
                page.ShowText(buf);
            }
            page.MoveTextPos(0, -25);

            for (int i = 0; i < len; i++) {
                string buf;

                float r = (float)i / (float)len;
                float b = 1 - ((float)i / (float)len);
                buf = samp_text.Substring(i, 1);

                page.SetRGBFill(r, 0, b);
                page.ShowText(buf);
            }
            page.MoveTextPos(0, -25);

            for (int i = 0; i < len; i++) {
                string buf;

                float b = (float)i / (float)len;
                float g = 1 - ((float)i / (float)len);
                buf = samp_text.Substring(i, 1);

                page.SetRGBFill(0, g, b);
                page.ShowText(buf);
           }
 
            page.EndText();

            int ypos = 450;

            /*
             * Font rendering mode
             */
            page.SetFontAndSize(font, 32);
            page.SetRGBFill(0.5f, 0.5f, 0);
            page.SetLineWidth(1.5f);

            /* PDF_FILL */
            ShowDescription(page, 60, ypos,
                "RenderingMode=PDF_FILL");
            page.SetTextRenderingMode(HPdfTextRenderingMode.HPDF_FILL);
            page.BeginText();
            page.TextOut(60, ypos, "ABCabc123");
            page.EndText();

            /*PDF_STROKE */
            ShowDescription(page, 60, ypos - 50,
                "RenderingMode=PDF_STROKE");
            page.SetTextRenderingMode(HPdfTextRenderingMode.HPDF_STROKE);
            page.BeginText();
            page.TextOut(60, ypos - 50, "ABCabc123");
            page.EndText();

            /*PDF_FILL_THEN_STROKE */
            ShowDescription(page, 60, ypos - 100,
                "RenderingMode=PDF_FILL_THEN_STROKE");
            page.SetTextRenderingMode(HPdfTextRenderingMode.HPDF_FILL_THEN_STROKE);
            page.BeginText();
            page.TextOut(60, ypos - 100, "ABCabc123");
            page.EndText();

            /*PDF_FILL_CLIPPING */
            ShowDescription(page, 60, ypos - 150,
                "RenderingMode=PDF_FILL_CLIPPING");
            page.GSave();
            page.SetTextRenderingMode(HPdfTextRenderingMode.HPDF_FILL_CLIPPING);
            page.BeginText();
            page.TextOut(60, ypos - 150, "ABCabc123");
            page.EndText();
            ShowStripePattern(page, 60, ypos - 150);
            page.GRestore();

            /*PDF_STROKE_CLIPPING */
            ShowDescription(page, 60, ypos - 200,
                "RenderingMode=PDF_STROKE_CLIPPING");
            page.GSave();
            page.SetTextRenderingMode(HPdfTextRenderingMode.HPDF_STROKE_CLIPPING);
            page.BeginText();
            page.TextOut(60, ypos - 200, "ABCabc123");
            page.EndText();
            ShowStripePattern(page, 60, ypos - 200);
            page.GRestore();

            /*PDF_FILL_STROKE_CLIPPING */
            ShowDescription(page, 60, ypos - 250,
                "RenderingMode=PDF_FILL_STROKE_CLIPPING");
            page.GSave();
            page.SetTextRenderingMode(HPdfTextRenderingMode.HPDF_FILL_STROKE_CLIPPING);
            page.BeginText();
            page.TextOut(60, ypos - 250, "ABCabc123");
            page.EndText();
            ShowStripePattern(page, 60, ypos - 250);
            page.GRestore();

            /*Reset text attributes */
            page.SetTextRenderingMode(HPdfTextRenderingMode.HPDF_FILL);
            page.SetRGBFill(0, 0, 0);
            page.SetFontAndSize(font, 30);


            /*
             * Rotating text
             */
            float angle1 = 30;                   /* A rotation of 30 degrees. */
            float rad1 = angle1 / 180 * 3.141592f; /* Calculate the radian value. */

            ShowDescription(page, 320, ypos - 60, "Rotating text");
            page.BeginText();
            page.SetTextMatrix((float)Math.Cos(rad1), (float)Math.Sin(rad1), 
                -(float)Math.Sin(rad1), (float)Math.Cos(rad1),
                330, ypos - 60);
            page.ShowText("ABCabc123");
            page.EndText();


            /*
             * Skewing text.
             */
            ShowDescription(page, 320, ypos - 120, "Skewing text");
            page.BeginText();

            angle1 = 10;
            float angle2 = 20;
            rad1 = angle1 / 180 * 3.141592f;
            float rad2 = angle2 / 180 * 3.141592f;

            page.SetTextMatrix(1, (float)Math.Tan(rad1), (float)Math.Tan(rad2), 1, 
                    320, ypos - 120);
            page.ShowText("ABCabc123");
            page.EndText();


            /*
             * scaling text (X direction)
             */
            ShowDescription(page, 320, ypos - 175, "Scaling text (X direction)");
            page.BeginText();
            page.SetTextMatrix(1.5f, 0, 0, 1, 320, ypos - 175);
            page.ShowText("ABCabc12");
            page.EndText();


            /*
             * scaling text (Y direction)
             */
            ShowDescription(page, 320, ypos - 250, "Scaling text (Y direction)");
            page.BeginText();
            page.SetTextMatrix(1, 0, 0, 2, 320, ypos - 250);
            page.ShowText("ABCabc123");
            page.EndText();


            /*
             * char spacing, word spacing
             */

            ShowDescription(page, 60, 140, "char-spacing 0");
            ShowDescription(page, 60, 100, "char-spacing 1.5");
            ShowDescription(page, 60, 60, "char-spacing 1.5, word-spacing 2.5");

            page.SetFontAndSize(font, 20);
            page.SetRGBFill(0.1f, 0.3f, 0.1f);

            /*char-spacing 0 */
            page.BeginText();
            page.TextOut(60, 140, samp_text2);
            page.EndText();

            /*char-spacing 1.5 */
            page.SetCharSpace(1.5f);

            page.BeginText();
            page.TextOut(60, 100, samp_text2);
            page.EndText();

            /*char-spacing 1.5, word-spacing 3.5 */
            page.SetWordSpace(2.5f);

            page.BeginText();
            page.TextOut(60, 60, samp_text2);
            page.EndText();

            /* save the document to a file */
            pdf.SaveToFile("TextDemo.pdf");

        } catch (Exception e) {
            Console.Error.WriteLine(e.Message);
        }
    }
}



