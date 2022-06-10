// \*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*
// Custom code for QuoteForm
// Created: 06/08/2015 1:35:58 PM
// \*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*
using System;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Windows.Forms;
using Erp.UI;
using Ice.Lib.Customization;
using Ice.Lib.ExtendedProps;
using Ice.Lib.Framework;
using Ice.Lib.Searches;
using Ice.UI.FormFunctions;

public class Script
{
    // \*\* Wizard Insert Location - Do Not Remove 'Begin/End Wizard Added Module Level Variables' Comments! \*\*
    // Begin Wizard Added Module Level Variables \*\*

    private EpiDataView edvQuoteDtl;
	
	
	
//Used in the inventory balance update epiviewnotification function below.  
    private int quoteLine = -2;
    private int quoteNum;
    private string partNum;

    // End Wizard Added Module Level Variables \*\*

    // Add Custom Module Level Variables Here \*\*

    public void InitializeCustomCode()
    {
        // \*\* Wizard Insert Location - Do not delete 'Begin/End Wizard Added Variable Initialization' lines \*\*
        // Begin Wizard Added Variable Initialization

        this.edvQuoteDtl = ((EpiDataView)(this.oTrans.EpiDataViews\["QuoteDtl"\]));
        this.edvQuoteDtl.EpiViewNotification += new EpiViewNotification(this.edvQuoteDtl\_EpiViewNotification);
        // End Wizard Added Variable Initialization

        // Begin Wizard Added Custom Method Calls

        // End Wizard Added Custom Method Calls
    }

    public void DestroyCustomCode()
    {
        // \*\* Wizard Insert Location - Do not delete 'Begin/End Wizard Added Object Disposal' lines \*\*
        // Begin Wizard Added Object Disposal

        this.edvQuoteDtl.EpiViewNotification -= new EpiViewNotification(this.edvQuoteDtl\_EpiViewNotification);
        this.edvQuoteDtl = null;
        // End Wizard Added Object Disposal

        // Begin Custom Code Disposal

        // End Custom Code Disposal
    }

    private void edvQuoteDtl\_EpiViewNotification(EpiDataView view, EpiNotifyArgs args)
    {
                // UPDATES the inventory balances box on the quote line  

        // \*\* Argument Properties and Uses \*\*
        // view.dataView\[args.Row\]\["FieldName"\]
        // args.Row, args.Column, args.Sender, args.NotifyType
        // NotifyType.Initialize, NotifyType.AddRow, NotifyType.DeleteRow, NotifyType.InitLastView, NotifyType.InitAndResetTreeNodes
        if ((args.NotifyType == EpiTransaction.NotifyType.Initialize))
        {

            //the below if statement checks to make sure your on a new line, new part, or new quote before updating balances
            if ((args.Row > -1) & (quoteLine != args.Row || quoteNum != (int)view.dataView\[args.Row\]\["QuoteNum"\]) || partNum != (string)view.dataView\[args.Row\]\["PartNum"\]) )
            {
                
                //the current quote line, partnum and quote number are stored as variables
                //this function runs whenever the epiview is initialized which is a lot.  Storing these variables and using the above if statements prevents constant updates.  
                quoteLine = args.Row; 
                quoteNum = (int)view.dataView\[args.Row\]\["QuoteNum"\];
		partNum = (string)view.dataView\[args.Row\]\["PartNum"\];
                
                //A Foreign Key view to the partwarehouse was added using data tools.  
                EpiDataView PartWhseEDV = ((EpiDataView)oTrans.EpiDataViews\["ZZ\_WhseQty"\]);
                
                //The below if statement checks to make sure the part exists in the epicor part database. A Custom on the fly part will fail and the inventory balance update will not run because there is no balance for custom parts. 
                if(PartWhseEDV.Row > -1) 
                {
                    //Pull the quantities from the part warehouse table for this quote lines part
                    decimal QtyOnHand = (decimal)PartWhseEDV.dataView\[PartWhseEDV.Row\]\["OnHandQty"\];
                    decimal QtyDemand = (decimal)PartWhseEDV.dataView\[PartWhseEDV.Row\]\["DemandQty"\];

                    
                    //calculate availability
                    decimal QtyAvailable = QtyOnHand - QtyDemand;
                    
                    //store the results in the textboxes
                    OnHandTxt.Text = ((int)QtyOnHand).ToString();
                    AvailableTxt.Text = ((int)QtyAvailable).ToString(); 
                }
                else
                {
                    OnHandTxt.Text = "0";
                    AvailableTxt.Text = "0";
                }

                //Pull the quote line qty and assign it to ThisLine Text box
                decimal QtyQuoted = (decimal)view.dataView\[args.Row\]\["OrderQty"\];
                ThisLineTxt.Text = ((int)QtyQuoted).ToString();
            }

        }
    }

}