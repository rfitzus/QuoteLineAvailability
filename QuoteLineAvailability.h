// **************************************************
// Custom code for QuoteForm
// Created: 06/08/2015 1:35:58 PM
// **************************************************

extern alias Erp_Contracts_BO_QuoteDtlSearch;
extern alias Erp_Contracts_BO_Quote;
extern alias Erp_Contracts_BO_Customer;
extern alias Erp_Contracts_BO_AlternatePart;
extern alias Erp_Contracts_BO_Part;
extern alias Erp_Contracts_BO_Vendor;
extern alias Erp_Contracts_BO_VendorPPSearch;
extern alias Erp_Contracts_BO_ShipTo;

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

////Added These Using References
using Ice.Core;
using Erp.Adapters;	

//CUSTOM ASSEMBLIES ADDED - Tools > Assembly Reference Manager > Add Customer Reference
//Erp.Adapter.CustCnt
//Ice.Core.Session

public class Script
{
	// ** Wizard Insert Location - Do Not Remove 'Begin/End Wizard Added Module Level Variables' Comments! **
	// Begin Wizard Added Module Level Variables **

	private EpiDataView edvQuoteDtl;

	//Used in the inventory balance update epiviewnotification function below.  
	private int quoteLine = -2;
	private int quoteNum;
	private string partNum;

	//Used in the QUOTE CONTACT CHECK IF INACTIVE function below.  
	private int quoteNumB = 0;

	//
	private DateTime savedTime = new DateTime(2009,12,9,10,0,0);

	// End Wizard Added Module Level Variables **

	// Add Custom Module Level Variables Here **

	public void InitializeCustomCode()
	{
		// ** Wizard Insert Location - Do not delete 'Begin/End Wizard Added Variable Initialization' lines **
		// Begin Wizard Added Variable Initialization

		this.edvQuoteDtl = ((EpiDataView)(this.oTrans.EpiDataViews["QuoteDtl"]));
		this.edvQuoteDtl.EpiViewNotification += new EpiViewNotification(this.edvQuoteDtl_EpiViewNotification);
		// End Wizard Added Variable Initialization

		// Begin Wizard Added Custom Method Calls

		CreateRowRuleQuoteHedTermsCodeNotEqual_N30();;
		CreateRowRuleQuoteHedQuotedEquals_True();;
		// End Wizard Added Custom Method Calls
	}

	public void DestroyCustomCode()
	{
		// ** Wizard Insert Location - Do not delete 'Begin/End Wizard Added Object Disposal' lines **
		// Begin Wizard Added Object Disposal

		this.edvQuoteDtl.EpiViewNotification -= new EpiViewNotification(this.edvQuoteDtl_EpiViewNotification);
		this.edvQuoteDtl = null;
		// End Wizard Added Object Disposal

		// Begin Custom Code Disposal

		// End Custom Code Disposal
	}


	private void edvQuoteDtl_EpiViewNotification(EpiDataView view, EpiNotifyArgs args)
	{

		// UPDATES the inventory balances box on the quote line  


		// ** Argument Properties and Uses **
		// view.dataView[args.Row]["FieldName"]
		// args.Row, args.Column, args.Sender, args.NotifyType
		// NotifyType.Initialize, NotifyType.AddRow, NotifyType.DeleteRow, NotifyType.InitLastView, NotifyType.InitAndResetTreeNodes
		if ((args.NotifyType == EpiTransaction.NotifyType.Initialize))
		{
			//the below if statement checks to make sure you are on a new line or new quote before updating balances
			if ((args.Row > -1) & (quoteLine != args.Row || quoteNum != (int)view.dataView[args.Row]["QuoteNum"] || partNum != (string)view.dataView[args.Row]["PartNum"]  ) )
			{
				
				//the current quote line and quote number are stored as variables
				//this function runs whenever the epiview is initialized which is a lot.  Storing these variables and using the above if statements prevents constant updates.  
				quoteLine = args.Row; 
				quoteNum = (int)view.dataView[args.Row]["QuoteNum"];
				partNum = (string)view.dataView[args.Row]["PartNum"];
				
				//A Foreign Key view to the partwarehouse was added using data tools.  
				EpiDataView PartWhseEDV = ((EpiDataView)oTrans.EpiDataViews["ZZ_WhseQty"]);
				
				//The below if statement checks to make sure the part exists in the epicor database. A Custom on the fly part will fail and the inventory balance update will not run because there is no balance for custom parts. 
				if(PartWhseEDV.Row > -1) 
				{
					//Pull the quantities from the part warehouse table for this quote lines part
					decimal QtyOnHand = (decimal)PartWhseEDV.dataView[PartWhseEDV.Row]["OnHandQty"];
					decimal QtyDemand = (decimal)PartWhseEDV.dataView[PartWhseEDV.Row]["DemandQty"];


					
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
				decimal QtyQuoted = (decimal)view.dataView[args.Row]["OrderQty"];
				ThisLineTxt.Text = ((int)QtyQuoted).ToString();
			}
		}
	}

	private void CreateRowRuleQuoteHedTermsCodeNotEqual_N30()
	{
		// Description: N30Terms
		// **** begin autogenerated code ****
		RuleAction warningQuoteHed_TermsCode = RuleAction.AddControlSettings(this.oTrans, "QuoteHed.TermsCode", SettingStyle.Warning);
		RuleAction[] ruleActions = new RuleAction[] {
				warningQuoteHed_TermsCode};
		// Create RowRule and add to the EpiDataView.
		RowRule rrCreateRowRuleQuoteHedTermsCodeNotEqual_N30 = new RowRule("QuoteHed.TermsCode", RuleCondition.NotEqual, "N30", ruleActions);
		((EpiDataView)(this.oTrans.EpiDataViews["QuoteHed"])).AddRowRule(rrCreateRowRuleQuoteHedTermsCodeNotEqual_N30);
		// **** end autogenerated code ****
	}


	private void CreateRowRuleQuoteHedQuotedEquals_True()
	{
		// Description: ContactInactive
		// **** begin autogenerated code ****
		RuleAction[] ruleActions = new RuleAction[0];
		// Create RowRule and add to the EpiDataView.
		// Dummy Context Object
		object contextObject = null;
		RowRule rrCreateRowRuleQuoteHedQuotedEquals_True = new RowRule("QuoteHed.Quoted", RuleCondition.Equals, true, new RowRuleActionDelegate2(this.QuoteHedQuotedEqualsTrue_CustomRuleAction), contextObject);
		((EpiDataView)(this.oTrans.EpiDataViews["QuoteHed"])).AddRowRule(rrCreateRowRuleQuoteHedQuotedEquals_True);
		// **** end autogenerated code ****
	}

	private void QuoteHedQuotedEqualsTrue_CustomRuleAction(Ice.Lib.ExtendedProps.RowRuleDelegateArgs args)
	{
		// ** RowRuleDelegateArgs Properties: args.Arg1, args.Arg2, args.Context, args.Row
		// ** put custom Rule Action logic here

		String outputMsg = "";
			
		EpiDataView QuoteCntView = ((EpiDataView)oTrans.EpiDataViews["QuoteCnt"]);
		
		foreach(DataRow dr in QuoteCntView.dataView.Table.Rows)
			{
				CustCntAdapter custCntAdpt = new CustCntAdapter(this.oTrans);
				custCntAdpt.BOConnect();
		
				custCntAdpt.GetByID((int)dr["CustNum"],(string)dr["ShipToNum"],(int)dr["ConNum"]);
		
				if(Convert.ToBoolean(custCntAdpt.CustCntData.CustCnt.Rows[0]["Inactive"]) == true) 
				{
					outputMsg += (string)dr["Name"] + " is set to INACTIVE under the Contacts tab in Customer Maintenance.\n\n";
				}
				else
				{
					//MessageBox.Show((string)dr["Name"] + " is set to ACTIVE under the Contacts tab in Customer Maintenance.");
				}
			
				//Cleanup
				custCntAdpt.Dispose();
			}
		
			if(outputMsg != "" && quoteNumB != (int)args.Row["QuoteNum"]) 
			{
				MessageBox.Show(outputMsg, "Inactive Contact Warning",MessageBoxButtons.OK,MessageBoxIcon.Warning);
				quoteNumB = (int)args.Row["QuoteNum"];
			}



		//Check if each line has a Product Group	
		EpiDataView QuoteDtl = ((EpiDataView)oTrans.EpiDataViews["QuoteDtl"]);

		DateTime now = DateTime.Now;

		if(now > savedTime) {

		string outputMsgB = "";

		foreach(DataRow dr in QuoteDtl.dataView.Table.Rows)
			{
				if((string)dr["ProdCode"] == "") 
				{
					outputMsgB += "Line " + (int)dr["QuoteLine"] + "-" + (string)dr["PartNum"] + "\n";
				}
			}
		if(outputMsgB != "") MessageBox.Show("The following lines are missing Product Groups:\n\n" + outputMsgB,"Product Group",MessageBoxButtons.OK,MessageBoxIcon.Warning);

		savedTime = DateTime.Now.AddMinutes(1);
		}
	}

	private void QuoteForm_Load(object sender, EventArgs args)
	{
		// Add Event Handler Code


		if (QuoteForm.LaunchFormOptions != null) 
    	{ 
			if (QuoteForm.LaunchFormOptions.ContextValue != null) 
    		{

				string Lfo = QuoteForm.LaunchFormOptions.ContextValue.ToString();
	
				EpiDataView QuoteHed = ((EpiDataView)oTrans.EpiDataViews["QuoteHed"]);
	        	this.oTrans.GetNewQuoteHed(); //Create New Quote
				
				//QuoteHed.dataView[QuoteHed.Row].BeginEdit(); 
				
				QuoteHed.dataView[QuoteHed.Row]["CustomerCustID"] =  Lfo;
			}
    	}
	}

}
